#ifndef __MAIN_FRAME_H__
#define __MAIN_FRAME_H__

#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/progdlg.h>
#include <archive.h>
#include <archive_entry.h>
#include <vector>

class MainFrame : public wxFrame {
public:
  MainFrame();
  ~MainFrame();

private:
  wxFilePickerCtrl *filePicker;
  wxNotebook *notebook;
  wxTreeCtrl *dirTree;

  void OnFileSelected(wxFileDirPickerEvent &event);
  void BuildDirectoryTree(wxProgressDialog *progdlg);
  void AddTreeItemsFromPath(const wxTreeItemId &parent, const wxString &path);

  wxDECLARE_EVENT_TABLE();

  archive *archive = nullptr;
  std::vector<archive_entry *> entries;
  std::vector<wxString> entryPaths;
};

#endif
