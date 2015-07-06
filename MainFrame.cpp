#include "MainFrame.h"
#include <algorithm>
#include <wx/log.h>
#include <wx/mstream.h>
#include <wx/button.h>
#include "ImageViewPanel.h"

enum MainFrameIds { ID_DIRECTORY_TREE = 1, ID_IMAGE_BUTTON };

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "ZipPicView") {
  auto statusBar = CreateStatusBar();
  SetStatusText("Welcome to ZipPicView!");

  auto outerSizer = new wxBoxSizer(wxVERTICAL);

  notebook = new wxNotebook(this, wxID_ANY);
  filePicker = new wxFilePickerCtrl(this, wxID_OPEN, wxEmptyString,
                                    wxFileSelectorPromptStr, "*.zip");
  filePicker->Bind(wxEVT_FILEPICKER_CHANGED, &MainFrame::OnFileSelected, this,
                   wxID_OPEN);
  if (filePicker->HasTextCtrl())
    filePicker->GetTextCtrl()->Disable();

  outerSizer->Add(filePicker, 0, wxEXPAND | wxALL, 10);
  outerSizer->Add(notebook, 1, wxEXPAND | wxALL, 10);

  splitter = new wxSplitterWindow(notebook, wxID_ANY);
  dirTree = new wxTreeCtrl(
      splitter, ID_DIRECTORY_TREE, wxDefaultPosition, wxDefaultSize,
      wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT | wxTR_ROW_LINES | wxTR_SINGLE);
  dirTree->Bind(wxEVT_TREE_SEL_CHANGED, &MainFrame::OnTreeSelectionChanged,
                this, ID_DIRECTORY_TREE);
  auto rightWindow = new wxScrolledWindow(splitter, wxID_ANY);
  auto grid = new wxGridSizer(5);
  rightWindow->SetSizer(grid);
  rightWindow->SetScrollRate(10, 10);
  rightWindow->Bind(wxEVT_SIZE, &MainFrame::OnGridPanelSize, this);
  splitter->SplitVertically(dirTree, rightWindow, 250);

  notebook->AddPage(splitter, "Browse");

  SetSizer(outerSizer);
}

MainFrame::~MainFrame() {
  if (zipFile)
    zip_close(zipFile);
}

void MainFrame::OnFileSelected(wxFileDirPickerEvent &event) {
  auto path = event.GetPath();

  int error;
  auto zipFile = zip_open(path, ZIP_RDONLY, &error);

  if (zipFile == nullptr) {
    throw error;
  }

  if (MainFrame::zipFile) {
    paths.clear();
    zip_close(MainFrame::zipFile);
  }

  auto progressDlg = new wxProgressDialog("Loading", "Please Wait");
  MainFrame::zipFile = zipFile;

  int num_entries = zip_get_num_entries(zipFile, ZIP_FL_UNCHANGED);
  for (int i = 0; i < num_entries; i++) {
    auto path = wxString(zip_get_name(zipFile, i, ZIP_FL_UNCHANGED));
    paths.push_back(path);
  }

  BuildDirectoryTree();
  dirTree->ExpandAll();
  progressDlg->Update(100);
  dirTree->UnselectAll();
  dirTree->SelectItem(dirTree->GetRootItem());
}

void MainFrame::BuildDirectoryTree() {
  dirTree->DeleteAllItems();
  auto root = dirTree->AddRoot("/");
  for (auto path : paths) {
    if (!path.EndsWith('/'))
      continue;
    AddTreeItemsFromPath(root, path);
  }
}

void MainFrame::AddTreeItemsFromPath(const wxTreeItemId &parent,
                                     const wxString &path) {
  auto name = path.BeforeFirst('/');

  wxTreeItemIdValue cookie;
  auto child = dirTree->GetFirstChild(parent, cookie);
  for (; child.IsOk(); child = dirTree->GetNextChild(parent, cookie)) {
    if (dirTree->GetItemText(child) == name)
      break;
  }
  if (!child.IsOk()) {
    child = dirTree->AppendItem(parent, name);
  }

  auto childPath = path.AfterFirst('/');

  if (childPath != "")
    AddTreeItemsFromPath(child, childPath);
}

std::vector<wxString> MainFrame::GetFileEntries(const wxString &parent) {
  std::vector<wxString> output;
  for (auto path : paths) {
    if (path.EndsWith('/'))
      continue;

    wxString childPath;
    if (!path.StartsWith(parent, &childPath))
      continue;

    if (childPath.Contains('/'))
      continue;

    if (!childPath.EndsWith(".jpg") && !childPath.EndsWith(".jpeg") &&
        !childPath.EndsWith(".png"))
      continue;

    output.push_back(path);
  }

  std::sort(output.begin(), output.end());

  return output;
}

void MainFrame::OnTreeSelectionChanged(wxTreeEvent &event) {
  auto progressDlg = new wxProgressDialog("Loading", "Please Wait");

  auto treeItemId = event.GetItem();
  auto rootId = dirTree->GetRootItem();
  wxString path;
  progressDlg->Pulse();
  for (auto id = treeItemId; id != rootId; id = dirTree->GetItemParent(id)) {
    path = dirTree->GetItemText(id) + "/" + path;
  }

  auto fileEntries = GetFileEntries(path);
  progressDlg->Pulse();
  auto gridPanel = dynamic_cast<wxScrolledWindow *>(splitter->GetWindow2());
  auto grid = gridPanel->GetSizer();
  grid->Clear(true);

  for (auto path : fileEntries) {

    auto image = LoadImage(path);

    auto button = new wxButton(gridPanel, wxID_ANY);
    button->Bind(wxEVT_BUTTON, &MainFrame::OnImageButtonClick, this);

    int longerSide = image.GetWidth() > image.GetHeight() ? image.GetWidth()
                                                          : image.GetHeight();
    int width = 200 * image.GetWidth() / longerSide;
    int height = 200 * image.GetHeight() / longerSide;
    auto thumbnailImage = image.Scale(width, height, wxIMAGE_QUALITY_HIGH);

    button->SetBitmap(thumbnailImage, wxBOTTOM);
    button->SetClientObject(new wxStringClientData(path));
    button->SetMinSize({250, 250});

    auto btnSizer = new wxBoxSizer(wxVERTICAL);
    btnSizer->Add(button, 0, wxEXPAND);
    btnSizer->Add(new wxStaticText(gridPanel, wxID_ANY, path.AfterLast('/')));

    grid->Add(btnSizer, 0, wxALL | wxEXPAND, 5);
    progressDlg->Pulse();
  }

  grid->FitInside(gridPanel);
  gridPanel->Scroll(0, 0);
  progressDlg->Update(100);
}

wxImage MainFrame::LoadImage(wxString innerFile) {
  struct zip_stat stat;
  zip_stat(zipFile, innerFile, 0, &stat);

  auto file = zip_fopen(zipFile, innerFile, 0);
  auto size = stat.size;
  auto buffer = new unsigned char[size];
  auto read = zip_fread(file, buffer, size);

  wxMemoryInputStream stream(buffer, size);

  wxImage output(stream);

  delete[] buffer;
  auto ok = output.IsOk();
  return output;
}

void MainFrame::OnImageButtonClick(wxCommandEvent &event) {
  auto button = dynamic_cast<wxButton *>(event.GetEventObject());
  auto clientData =
      dynamic_cast<wxStringClientData *>(button->GetClientObject());
  auto path = clientData->GetData();

  auto page = notebook->GetPageCount();

  auto bitmapCtl = new ImageViewPanel(notebook, LoadImage(path));
  notebook->AddPage(bitmapCtl, path.AfterLast('/'));
  notebook->SetSelection(page);
}

void MainFrame::OnGridPanelSize(wxSizeEvent &event) {
  auto grid = dynamic_cast<wxGridSizer *>(splitter->GetWindow2()->GetSizer());
  auto size = event.GetSize();
  int col = (size.GetWidth() / 250);
  grid->SetCols(col > 0 ? col : 1);

  grid->FitInside(splitter->GetWindow2());
}
