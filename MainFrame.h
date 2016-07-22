#ifndef __MAIN_FRAME_H__
#define __MAIN_FRAME_H__

#include <wx/filename.h>
#include <wx/filepicker.h>
#include <wx/gauge.h>
#include <wx/notebook.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

#include "Entry.h"
#include "ThumbnailLoadThread.h"
#include <map>
#include <memory>
#include <vector>

class MainFrame : public wxFrame {
public:
  MainFrame();

private:
  wxNotebook *notebook;
  wxTreeCtrl *dirTree;
  wxSplitterWindow *splitter;
  wxCheckBox *onTopChk;

  wxTextCtrl *currentFileCtrl;
  wxButton *dirBrowseBtn;
  wxButton *zipBrowseBtn;
  wxGauge *progress;
  wxStaticText *progressDescText;

  wxThreadIdType threadId;
  ThumbnailLoadThread *loadThread;
  std::vector<wxButton *> imgButtons;

  void OnTreeSelectionChanged(wxTreeEvent &event);
  void OnImageButtonClick(wxCommandEvent &event);
  void OnGridPanelSize(wxSizeEvent &event);
  void OnOnTopChecked(wxCommandEvent &event);
  void OnDirBrowsePressed(wxCommandEvent &event);
  void OnZipBrowsePressed(wxCommandEvent &event);
  void OnThumbnailLoadUpdated(wxThreadEvent &event);
  void OnThumbnailLoadDone(wxThreadEvent &event);

  void BuildDirectoryTree();
  void AddTreeItemsFromEntry(const wxTreeItemId &itemId, Entry *entry);

  void SetEntry(std::shared_ptr<Entry> entry);
  template <class T> void LoadEntryFromFile(const wxFileName &filename);

  std::shared_ptr<Entry> currentEntry;
  wxFileName filename;
};

#endif
