#include "MainFrame.h"
#include <algorithm>
#include <wx/log.h>
#include <wx/mstream.h>
#include "ThumbnailPanel.h"

enum MainFrameIds { ID_DIRECTORY_TREE = 1 };

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "ZipPicView") {
  auto statusBar = CreateStatusBar();
  SetStatusText("Welcome to ZipPicView!");

  auto outerSizer = new wxBoxSizer(wxVERTICAL);

  notebook = new wxNotebook(this, wxID_ANY);
  filePicker = new wxFilePickerCtrl(this, wxID_OPEN, wxEmptyString,
                                    wxFileSelectorPromptStr, "*.zip");
  outerSizer->Add(filePicker, 0, wxALL, 10);
  outerSizer->Add(notebook, 1, wxEXPAND | wxALL, 10);

  splitter = new wxSplitterWindow(notebook, wxID_ANY);
  dirTree = new wxTreeCtrl(
      splitter, ID_DIRECTORY_TREE, wxDefaultPosition, wxDefaultSize,
      wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT | wxTR_ROW_LINES | wxTR_SINGLE);

  auto rightWindow = new wxWindow(splitter, wxID_ANY);
  splitter->SplitVertically(dirTree, rightWindow, 250);

  notebook->AddPage(splitter, "Browse");
  notebook->AddPage(new wxButton(notebook, wxID_ANY, "Test"), "2");

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
    zip_close(zipFile);
  }

  auto progressDlg = new wxProgressDialog("Loading", "Please Wait");
  MainFrame::zipFile = zipFile;

  int num_entries = zip_get_num_entries(zipFile, ZIP_FL_UNCHANGED);
  for (int i = 0; i < num_entries; i++) {
    auto path = wxString(zip_get_name(zipFile, i, ZIP_FL_UNCHANGED));
    paths.push_back(path);
    progressDlg->Pulse();
  }

  BuildDirectoryTree(progressDlg);
  dirTree->ExpandAll();
  progressDlg->Update(100);
  dirTree->SelectItem(dirTree->GetRootItem());
}

void MainFrame::BuildDirectoryTree(wxProgressDialog *progdlg) {
  dirTree->DeleteAllItems();
  auto root = dirTree->AddRoot("/");
  for (auto path : paths) {
    progdlg->Pulse();
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
  auto treeItemId = event.GetItem();
  auto rootId = dirTree->GetRootItem();
  wxString path;

  for (auto id = treeItemId; id != rootId; id = dirTree->GetItemParent(id)) {
    path = dirTree->GetItemText(id) + "/" + path;
  }

  auto fileEntries = GetFileEntries(path);
  auto grid = new wxGridSizer(5);
  auto rightWindow = new wxWindow(splitter, wxID_ANY);
  splitter->ReplaceWindow(splitter->GetWindow2(), rightWindow);
  for (auto path : fileEntries) {
    auto image = LoadImage(path);
    auto ctrl =
        new ThumbnailPanel(rightWindow, wxID_ANY, path.AfterLast('/'), image);
    grid->Add(ctrl);
  }

  rightWindow->SetSizerAndFit(grid);
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

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_FILEPICKER_CHANGED(wxID_OPEN, MainFrame::OnFileSelected)
  EVT_TREE_SEL_CHANGED(ID_DIRECTORY_TREE, MainFrame::OnTreeSelectionChanged)
wxEND_EVENT_TABLE();
// clang-format on
