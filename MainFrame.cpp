#include "MainFrame.h"
#include <wx/splitter.h>
#include <wx/log.h>

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "ZipPicView") {
  auto statusBar = CreateStatusBar();
  SetStatusText("Welcome to ZipPicView!");

  auto outerSizer = new wxBoxSizer(wxVERTICAL);

  notebook = new wxNotebook(this, wxID_ANY);
  filePicker = new wxFilePickerCtrl(this, wxID_OPEN, wxEmptyString,
                                    wxFileSelectorPromptStr, "*.zip");
  outerSizer->Add(filePicker, 0, wxALL, 10);
  outerSizer->Add(notebook, 1, wxEXPAND | wxALL, 10);

  auto splitter = new wxSplitterWindow(notebook, wxID_ANY);
  dirTree = new wxTreeCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT |
                               wxTR_ROW_LINES | wxTR_SINGLE);
  splitter->SplitVertically(dirTree, new wxButton(splitter, wxID_ANY, "Test"),
                            250);

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
    entryPaths.push_back(entryPath);
    progressDlg->Pulse();
    // wxLogDebug("Entry: %s", entryPath);
  }

  BuildDirectoryTree(progressDlg);
  dirTree->ExpandAll();
  progressDlg->Update(100);
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
  wxLogDebug("Current Name : %s ", name);
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
  wxLogDebug("Child Path : %s", childPath);

  if (childPath != "")
    AddTreeItemsFromPath(child, childPath);
}

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_FILEPICKER_CHANGED(wxID_OPEN, MainFrame::OnFileSelected)
wxEND_EVENT_TABLE();
// clang-format on
