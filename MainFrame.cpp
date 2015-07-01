#include "MainFrame.h"
#include <algorithm>
#include <wx/log.h>

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
  if (archive)
    archive_read_close(archive);
}

void MainFrame::OnFileSelected(wxFileDirPickerEvent &event) {
  auto path = event.GetPath();
  wxLogDebug("Reading File %s", path);

  auto archive = archive_read_new();
  archive_read_support_filter_all(archive);
  archive_read_support_format_all(archive);

  auto result = archive_read_open_filename(archive, path, 10240);
  if (result != ARCHIVE_OK) {
    throw result;
  }
  if (MainFrame::archive) {
    for (auto entry : entries) {
      archive_entry_free(entry);
    }
    archive_read_close(MainFrame::archive);
  }

  auto progressDlg = new wxProgressDialog("Loading", "Please Wait");
  MainFrame::archive = archive;

  archive_entry *entry;
  entries.clear();
  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
    wxString entryPath = archive_entry_pathname(entry);
    entries.push_back(archive_entry_clone(entry));

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
  for (auto entry : entries) {
    progdlg->Pulse();
    if (archive_entry_filetype(entry) != AE_IFDIR)
      continue;
    AddTreeItemsFromPath(root, wxString(archive_entry_pathname(entry)));
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

std::vector<archive_entry *> MainFrame::GetFileEntries(const wxString &path) {
  std::vector<archive_entry *> output;
  for (auto entry : entries) {
    if (archive_entry_filetype(entry) != AE_IFREG)
      continue;

    wxString entryPath = wxString(archive_entry_pathname(entry));

    wxString childPath;
    if (!entryPath.StartsWith(path, &childPath))
      continue;

    if (childPath.Contains('/'))
      continue;

    if (!childPath.EndsWith(".jpg") && !childPath.EndsWith(".jpeg") &&
        !childPath.EndsWith(".png"))
      continue;

    output.push_back(entry);
  }

  std::sort(output.begin(), output.end(),
            [](archive_entry *entry1, archive_entry *entry2) {
              return wxString(archive_entry_pathname(entry1)) <
                     wxString(archive_entry_pathname(entry2));
            });

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
  for (auto entry : fileEntries) {
    auto text = new wxStaticText(rightWindow, wxID_ANY,
                                 wxString(archive_entry_pathname(entry)));
    grid->Add(text);
  }

  rightWindow->SetSizer(grid);
}

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_FILEPICKER_CHANGED(wxID_OPEN, MainFrame::OnFileSelected)
  EVT_TREE_SEL_CHANGED(ID_DIRECTORY_TREE, MainFrame::OnTreeSelectionChanged)
wxEND_EVENT_TABLE();
// clang-format on
