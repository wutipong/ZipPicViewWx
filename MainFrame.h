#ifndef __MAIN_FRAME_H__
#define __MAIN_FRAME_H__

#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>

#include <zip.h>
#include <vector>
#include <map>
#include "Entry.h"

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

  void OnTreeSelectionChanged(wxTreeEvent &event);
  void OnImageButtonClick(wxCommandEvent &event);
  void OnGridPanelSize(wxSizeEvent &event);
  void OnOnTopChecked(wxCommandEvent &event);
  void OnDirBrowsePressed(wxCommandEvent& event);
  void OnZipBrowsePressed(wxCommandEvent& event);

  void BuildDirectoryTree();
  void AddTreeItemsFromEntry(const wxTreeItemId &itemId, Entry *entry);

  Entry *entry;
};

#endif
