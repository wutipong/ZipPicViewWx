#include "MainFrame.h"
#include "ArchiveExtractor.h"
#include "FileEntry.h"
#include "ImageViewPanel.h"
#include "ZipEntry.h"
#include "res/appicon.xpm"
#include <algorithm>
#include <wx/button.h>
#include <wx/log.h>
#include <wx/mstream.h>

enum MainFrameIds { ID_DIRECTORY_TREE = 1, ID_IMAGE_BUTTON };

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "ZipPicView") {
  auto panel = new wxPanel(this);

  auto outerSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(outerSizer);
  auto toolSizer = new wxBoxSizer(wxHORIZONTAL);
  onTopChk = new wxCheckBox(panel, wxID_ANY, "On Top");
  onTopChk->Bind(wxEVT_CHECKBOX, &MainFrame::OnOnTopChecked, this);
  onTopChk->SetToolTip("Force the application to stay on top.");
  notebook = new wxNotebook(panel, wxID_ANY);

  currentFileCtrl =
      new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
                     wxDefaultSize, wxTE_READONLY);

  dirBrowseBtn = new wxButton(panel, wxID_ANY, "Directory...");
  dirBrowseBtn->Bind(wxEVT_BUTTON, &MainFrame::OnDirBrowsePressed, this);
  dirBrowseBtn->SetToolTip("Load images from a directory.");
  zipBrowseBtn = new wxButton(panel, wxID_ANY, "Archive...");
  zipBrowseBtn->Bind(wxEVT_BUTTON, &MainFrame::OnZipBrowsePressed, this);
  zipBrowseBtn->SetToolTip("Load images from a zip file.");

  toolSizer->Add(currentFileCtrl, 1, wxEXPAND);
  toolSizer->Add(dirBrowseBtn, 0, wxEXPAND | wxLEFT | wxALIGN_BOTTOM, 5);
  toolSizer->Add(zipBrowseBtn, 0, wxEXPAND | wxLEFT | wxALIGN_BOTTOM, 5);
  toolSizer->Add(onTopChk, 0, wxEXPAND | wxLEFT | wxALIGN_BOTTOM, 5);

  auto statusBar = CreateStatusBar();
  statusBar->SetStatusText("Idle");
  /* progress = new wxGauge(panel, wxID_ANY, 100);
  progressDescText = new wxStaticText(panel, wxID_ANY, "Idle");
  progressDescText->SetMinSize({250, 20});

  auto progressSizer = new wxBoxSizer(wxHORIZONTAL);
  progressSizer->AddStretchSpacer();
  progressSizer->Add(progressDescText, 0, wxEXPAND | wxALL | wxALIGN_RIGHT, 5);
  progressSizer->AddSpacer(5);
  progressSizer->Add(progress, 0, wxEXPAND | wxALL | wxALIGN_RIGHT, 5);*/

  outerSizer->Add(toolSizer, 0, wxEXPAND | wxALL, 5);
  outerSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);
  // outerSizer->Add(progressSizer, 0);

  splitter = new wxSplitterWindow(notebook, wxID_ANY);
  splitter->Bind(wxEVT_SPLITTER_DOUBLECLICKED,
                 [](wxSplitterEvent &event) { event.Veto(); }, wxID_ANY);
  dirTree =
      new wxTreeCtrl(splitter, ID_DIRECTORY_TREE, wxDefaultPosition,
                     wxDefaultSize, wxTR_SINGLE | wxTR_FULL_ROW_HIGHLIGHT);
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
  rightWindow->SetWindowStyle(wxBORDER_SIMPLE);
  rightWindow->Bind(wxEVT_SIZE, &MainFrame::OnGridPanelSize, this);
  splitter->SplitVertically(dirTree, rightWindow, 250);

  notebook->AddPage(splitter, "Browse");

  Bind(wxEVT_COMMAND_THMBTREAD_UPDATE, &MainFrame::OnThumbnailLoadUpdated,
       this);
  Bind(wxEVT_COMMAND_THMBTREAD_DONE, &MainFrame::OnThumbnailLoadDone, this);

  SetMinSize({700, 480});
  SetSize({700, 480});
  SetIcons(wxICON(IDI_ICON1));
  SetWindowStyle(wxRESIZE_BORDER | wxDEFAULT_FRAME_STYLE);
}

void MainFrame::BuildDirectoryTree() {
  auto root = dirTree->AddRoot(currentEntry->Name(), -1, -1,
                               new EntryItemData(currentEntry.get()));
  AddTreeItemsFromEntry(root, currentEntry.get());
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
  auto treeItemId = event.GetItem();
  auto rootId = dirTree->GetRootItem();
  auto currentFileEntry =
      dynamic_cast<EntryItemData *>(dirTree->GetItemData(treeItemId))->Get();

  auto gridPanel = dynamic_cast<wxScrolledWindow *>(splitter->GetWindow2());

  if (loadThread) {
    loadThread->Delete(nullptr, wxTHREAD_WAIT_BLOCK);
    loadThread = nullptr;
  }

  gridPanel->Show(false);
  auto grid = gridPanel->GetSizer();
  grid->Clear(true);

  std::vector<Entry *> loadEntries;
  imgButtons.clear();
  for (int i = 0; i < currentFileEntry->Count(); ++i) {
    Entry *childEntry = (*currentFileEntry)[i];

    if (childEntry->IsDirectory())
      continue;
    auto ext = childEntry->Name().AfterLast('.').Lower();

    if (ext != "jpg" && ext != "jpeg" && ext != "png" && ext != "gif")
      continue;

    loadEntries.push_back(childEntry);
  }

  for (auto entry : loadEntries) {
    auto button = new wxButton(gridPanel, wxID_ANY, "", wxDefaultPosition,
                               wxDefaultSize, wxBU_EXACTFIT);
    imgButtons.push_back(button);
    button->Bind(wxEVT_BUTTON, &MainFrame::OnImageButtonClick, this);

    button->SetClientObject(new EntryItemData(entry));
    button->SetMinSize({250, 250});

    auto staticText = new wxStaticText(gridPanel, wxID_ANY, entry->Name());
    staticText->SetMaxSize({250, 50});

    auto btnSizer = new wxBoxSizer(wxVERTICAL);
    btnSizer->Add(button, 0, wxEXPAND);
    btnSizer->Add(staticText);

    grid->Add(btnSizer, 0, wxALL | wxEXPAND, 5);
  }

  /*progress->SetRange(loadEntries.size());
  progressDescText->SetLabelText(wxString::Format("Loading Thumbnail %i of %i",
                                                  1,
  (int)loadEntries.size()));*/

  GetStatusBar()->SetStatusText(wxString::Format("Loading Thumbnail %i of %i",
                                                 1, (int)loadEntries.size()));

  grid->FitInside(gridPanel);
  gridPanel->Show(true);
  gridPanel->Scroll(0, 0);

  gridPanel->Refresh();
  gridPanel->Update();

  loadThread = new ThumbnailLoadThread(this, loadEntries, currentEntry);
  loadThread->Run();
  threadId = loadThread->GetId();
}

void MainFrame::OnImageButtonClick(wxCommandEvent &event) {
  wxProgressDialog progress("Loading", "Now Loading", 4, this);
  auto button = dynamic_cast<wxButton *>(event.GetEventObject());
  auto clientData =
      dynamic_cast<wxStringClientData *>(button->GetClientObject());

  auto page = notebook->GetPageCount();
  progress.Update(1);
  auto childEntry =
      dynamic_cast<EntryItemData *>(button->GetClientObject())->Get();
  progress.Update(2);
  auto bitmapCtl = new ImageViewPanel(notebook, childEntry);
  progress.Update(3);
  notebook->AddPage(bitmapCtl, childEntry->Name());
  progress.Update(4);
  notebook->SetSelection(page);
}

void MainFrame::OnGridPanelSize(wxSizeEvent &event) {
  auto grid = dynamic_cast<wxGridSizer *>(splitter->GetWindow2()->GetSizer());
  auto size = event.GetSize();
  int col = (size.GetWidth() / 250);
  grid->SetCols(col > 0 ? col : 1);

  grid->FitInside(splitter->GetWindow2());
  splitter->Refresh();
  splitter->Update();
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
  wxDirDialog dlg(this, "Choose directory", filename.GetPath(),
                  wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
  if (dlg.ShowModal() == wxID_CANCEL)
    return;

  wxFileName filename = wxFileName::DirName(dlg.GetPath());
  LoadEntryFromFile<FileEntry>(filename);
}

void MainFrame::OnZipBrowsePressed(wxCommandEvent &event) {
  wxFileDialog dialog(this, _("Open Archive file"), "", "",
                      "Archive Files|*.rar;*.zip;*.7z|ZIP files"
                      "(*.zip)|*.zip|RAR files (*.rar)|*.rar|7z files "
                      "(*.7z)|*.7z|All Files| *.*",

                      wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() == wxID_CANCEL)
    return;

  auto path = dialog.GetPath();
  filename = path;

  if (filename.GetExt() == "zip")
    LoadEntryFromFile<ZipEntry>(filename);
  else
    LoadEntryFromFile<ArchiveExtractor>(filename);
}

template <class T>
void MainFrame::LoadEntryFromFile(const wxFileName &filename) {
  wxProgressDialog progress("Loading", "Now Loading", 4, this);

  auto entry = T::Create(filename, [&progress](){
      progress.Pulse();
  });
  if(entry == nullptr) {
    wxMessageBox("Failed to load file", "Error", wxICON_ERROR);
    return;
  }
  SetEntry(std::shared_ptr<Entry>(entry));
  progress.Pulse();
  currentFileCtrl->SetValue(filename.GetFullPath());
}
void MainFrame::SetEntry(std::shared_ptr<Entry> entry) {
  MainFrame::currentEntry = entry;

  dirTree->DeleteAllItems();

  BuildDirectoryTree();

  dirTree->ExpandAll();
  dirTree->UnselectAll();
  dirTree->SelectItem(dirTree->GetRootItem());

  while (notebook->GetPageCount() > 1) {
    notebook->DeletePage(1);
  }
}

void MainFrame::OnThumbnailLoadUpdated(wxThreadEvent &event) {
  auto data = event.GetPayload<ThumbnailData>();

  if (data.index >= data.total)
    return;

  if (data.id != threadId)
    return;

  /*
  progress->SetValue(data.index);
  progressDescText->SetLabelText(wxString::Format("Loading Thumbnail %i of %i",
                                                  data.index + 2, data.total));
  */
  GetStatusBar()->SetStatusText(wxString::Format("Loading Thumbnail %i of %i",
                                                 data.index + 1, data.total));
  if (data.index < imgButtons.size())
    imgButtons[data.index]->SetBitmap(data.image);
}

void MainFrame::OnThumbnailLoadDone(wxThreadEvent &event) {
  // progressDescText->SetLabelText("Idle");
  // progress->SetValue(progress->GetRange());

  if (event.GetExtraLong() != threadId)
    return;

  GetStatusBar()->SetStatusText("Idle");
  loadThread = nullptr;
}
