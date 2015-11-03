#include "MainFrame.h"
#include <algorithm>
#include <wx/log.h>
#include <wx/mstream.h>
#include <wx/button.h>
#include "ImageViewPanel.h"
#include "ZipEntry.h"
#include "FileEntry.h"

enum MainFrameIds { ID_DIRECTORY_TREE = 1, ID_IMAGE_BUTTON };

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "ZipPicView") {
  auto statusBar = CreateStatusBar();
  SetStatusText("Welcome to ZipPicView!");

  auto outerSizer = new wxBoxSizer(wxVERTICAL);
  auto toolSizer = new wxBoxSizer(wxHORIZONTAL);
  onTopChk = new wxCheckBox(this, wxID_ANY, "On Top");
  onTopChk->Bind(wxEVT_CHECKBOX, &MainFrame::OnOnTopChecked, this);
  notebook = new wxNotebook(this, wxID_ANY);

  currentFileCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  dirBrowseBtn = new wxButton(this, wxID_ANY, "Directory...");
  dirBrowseBtn->Bind(wxEVT_BUTTON, &MainFrame::OnDirBrowsePressed, this);
  zipBrowseBtn = new wxButton(this, wxID_ANY, "Zip...");
  zipBrowseBtn->Bind(wxEVT_BUTTON, &MainFrame::OnZipBrowsePressed, this);

  toolSizer->Add(currentFileCtrl, 1, wxEXPAND | wxALL);
  toolSizer->Add(dirBrowseBtn, 0, wxEXPAND | wxALL);
  toolSizer->Add(zipBrowseBtn, 0, wxEXPAND | wxALL);
  toolSizer->Add(onTopChk, 0, wxEXPAND | wxALL);

  outerSizer->Add(toolSizer, 0, wxEXPAND | wxALL, 5);
  outerSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

  splitter = new wxSplitterWindow(notebook, wxID_ANY);
  dirTree = new wxTreeCtrl(splitter, ID_DIRECTORY_TREE, wxDefaultPosition,
                           wxDefaultSize, wxTR_SINGLE | wxTR_NO_LINES |
          wxTR_FULL_ROW_HIGHLIGHT);
  dirTree->Bind(wxEVT_TREE_SEL_CHANGED, &MainFrame::OnTreeSelectionChanged,
                this, ID_DIRECTORY_TREE);
  dirTree->Bind(wxEVT_TREE_ITEM_COLLAPSING,
                [=](wxTreeEvent &event) { event.Veto(); }, ID_DIRECTORY_TREE);
  dirTree->SetMinSize({250, 250});

  auto rightWindow = new wxScrolledWindow(splitter, wxID_ANY);
  auto grid = new wxGridSizer(5);
  rightWindow->SetSizer(grid);
  rightWindow->SetScrollRate(10, 10);
  rightWindow->SetMinSize({250, 250});
  rightWindow->Bind(wxEVT_SIZE, &MainFrame::OnGridPanelSize, this);
  splitter->SplitVertically(dirTree, rightWindow, 250);

  notebook->AddPage(splitter, "Browse");

  SetMinSize({640, 480});
  SetSizer(outerSizer);
}

void MainFrame::BuildDirectoryTree() {
  dirTree->DeleteAllItems();

  auto root = dirTree->AddRoot(entry->Name(), -1, -1, new EntryItemData(entry));
  AddTreeItemsFromEntry(root, entry);
}

void MainFrame::AddTreeItemsFromEntry(const wxTreeItemId &itemId,
                                      Entry *entry) {
  for (auto childEntry : *entry) {
    if (!childEntry->IsDirectory())
      return;

    auto child = dirTree->AppendItem(itemId, childEntry->Name(), -1, -1,
                                     new EntryItemData(childEntry));

    AddTreeItemsFromEntry(child, childEntry);
  }
}

void MainFrame::OnTreeSelectionChanged(wxTreeEvent &event) {
  auto progressDlg = new wxProgressDialog("Loading", "Please Wait");

  auto treeItemId = event.GetItem();
  auto rootId = dirTree->GetRootItem();
  auto currentFileEntry =
      dynamic_cast<EntryItemData *>(dirTree->GetItemData(treeItemId))->Get();

  progressDlg->Pulse();
  auto gridPanel = dynamic_cast<wxScrolledWindow *>(splitter->GetWindow2());
  gridPanel->Show(false);
  auto grid = gridPanel->GetSizer();
  grid->Clear(true);

  for (auto childEntry : *entry) {
    if (childEntry->IsDirectory())
      return;

    auto name = childEntry->Name();
    if (!(name.EndsWith(".jpg") || name.EndsWith(".jpeg") ||
        name.EndsWith(".png") || name.EndsWith(".gif")))
      return;

    auto image = childEntry->LoadImage();
    auto button = new wxButton(gridPanel, wxID_ANY);
    button->Bind(wxEVT_BUTTON, &MainFrame::OnImageButtonClick, this);

    int longerSide = image.GetWidth() > image.GetHeight() ? image.GetWidth()
                                                          : image.GetHeight();
    int width = 200 * image.GetWidth() / longerSide;
    int height = 200 * image.GetHeight() / longerSide;
    auto thumbnailImage = image.Scale(width, height, wxIMAGE_QUALITY_HIGH);

    button->SetBitmap(thumbnailImage, wxBOTTOM);
    button->SetClientObject(new EntryItemData(childEntry));
    button->SetMinSize({250, 250});

    auto btnSizer = new wxBoxSizer(wxVERTICAL);
    btnSizer->Add(button, 0, wxEXPAND);
    btnSizer->Add(new wxStaticText(gridPanel, wxID_ANY, childEntry->Name()));

    grid->Add(btnSizer, 0, wxALL | wxEXPAND, 5);
    progressDlg->Pulse();
  }

  grid->FitInside(gridPanel);
  gridPanel->Show(true);
  gridPanel->Scroll(0, 0);
  progressDlg->Update(100);
}

void MainFrame::OnImageButtonClick(wxCommandEvent &event) {
  auto button = dynamic_cast<wxButton *>(event.GetEventObject());
  auto clientData =
      dynamic_cast<wxStringClientData *>(button->GetClientObject());

  auto page = notebook->GetPageCount();
  auto childEntry =
      dynamic_cast<EntryItemData *>(button->GetClientObject())->Get();
  auto bitmapCtl = new ImageViewPanel(notebook, childEntry->LoadImage());
  notebook->AddPage(bitmapCtl, childEntry->Name());
  notebook->SetSelection(page);
}

void MainFrame::OnGridPanelSize(wxSizeEvent &event) {
  auto grid = dynamic_cast<wxGridSizer *>(splitter->GetWindow2()->GetSizer());
  auto size = event.GetSize();
  int col = (size.GetWidth() / 250);
  grid->SetCols(col > 0 ? col : 1);

  grid->FitInside(splitter->GetWindow2());
}

void MainFrame::OnOnTopChecked(wxCommandEvent &event) {
  auto style = GetWindowStyle();

  if (onTopChk->IsChecked()) {
    style += wxSTAY_ON_TOP;
  } else {
    style -= wxSTAY_ON_TOP;
  }
  SetWindowStyle(style);
}

void MainFrame::OnDirBrowsePressed(wxCommandEvent &event) {
  wxDirDialog dlg(NULL, "Choose directory", "",
                  wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
  if (dlg.ShowModal() == wxID_CANCEL)
    return;

  auto path = dlg.GetPath();
  auto progressDlg = new wxProgressDialog("Loading", "Please Wait");

  if (entry) {
    delete entry;
    entry = nullptr;
  }

  wxFileName filename(path);
  entry = FileEntry::Create(filename);

  BuildDirectoryTree();

  dirTree->UnselectAll();
  dirTree->SelectItem(dirTree->GetRootItem());
  dirTree->ExpandAll();

  progressDlg->Update(100);
  currentFileCtrl->SetLabelText(filename.GetFullPath());
}

void MainFrame::OnZipBrowsePressed(wxCommandEvent &event) {
  wxFileDialog
      dialog(this, _("Open ZIP file"), "", "",
             "ZIP files (*.zip)|*.zip", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() == wxID_CANCEL)
    return;

  auto path = dialog.GetPath();
  auto progressDlg = new wxProgressDialog("Loading", "Please Wait");
  progressDlg->Update(0);
  if (entry) {
    delete entry;
    entry = nullptr;
  }

  wxFileName filename(path);

  int error;
  auto zipFile = zip_open(path, ZIP_RDONLY, &error);

  if (zipFile == nullptr) {
    progressDlg->Update(100);
    throw error;
  }

  entry = ZipEntry::Create(zipFile);

  BuildDirectoryTree();

  dirTree->UnselectAll();
  dirTree->SelectItem(dirTree->GetRootItem());
  dirTree->ExpandAll();

  progressDlg->Update(100);
  progressDlg->Close(true);
  currentFileCtrl->SetLabelText(filename.GetFullPath());
}