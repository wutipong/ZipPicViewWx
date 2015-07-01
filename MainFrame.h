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

class MainFrame : public wxFrame {
public:
  MainFrame();
  ~MainFrame();

private:
  wxFilePickerCtrl *filePicker;
  wxNotebook *notebook;
  wxTreeCtrl *dirTree;
  wxSplitterWindow *splitter;

  void OnFileSelected(wxFileDirPickerEvent &event);
  void OnTreeSelectionChanged(wxTreeEvent &event);

  void BuildDirectoryTree(wxProgressDialog *progdlg);
  void AddTreeItemsFromPath(const wxTreeItemId &parent, const wxString &path);

  std::vector<wxString> GetFileEntries(const wxString &parent);
  wxImage LoadImage(wxString innerFile);
  wxDECLARE_EVENT_TABLE();

  zip_t *zipFile = nullptr;
  std::vector<wxString> paths;
};

#endif
