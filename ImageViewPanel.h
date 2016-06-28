#ifndef __IMAGE_VIEW_PANEL__
#define __IMAGE_VIEW_PANEL__

#include <wx/button.h>
#include <wx/spinctrl.h>
#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "Entry.h"

class ImageViewPanel : public wxPanel {
public:
  ImageViewPanel(wxWindow *parent, Entry *entry, wxWindowID id = wxID_ANY,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = wxTAB_TRAVERSAL,
                 const wxString &name = "ImageViewPanel");

private:
  wxButton *btnClose;
  wxStaticBitmap *bitmap;
  wxScrolledWindow *scrollPanel;
  wxBoxSizer *scrollSizer;
  wxImage image;
  wxSpinCtrl *spnScale;
  wxButton *btnNext;
  wxButton *btnPrev;
  wxToggleButton *btnAuto;
  wxSpinCtrl *spnRefreshTime;
  wxToggleButton *btnFitSize;
  wxButton *btnActualSize;

  std::vector<Entry *> entries;
  std::vector<Entry *>::const_iterator entryIter;

  void OnCloseButtonClick(wxCommandEvent &event);
  void OnBtnFitSizeToggle(wxCommandEvent &event);
  void OnBtnActualSizePressed(wxCommandEvent &event);
  void OnScaleValueChanged(wxSpinEvent &event);
  void OnNextButtonClick(wxCommandEvent &event);
  void OnPrevButtonClick(wxCommandEvent &event);

  void RefreshImage(const int &percentage = 100);

  void FitImage();
  void SetImageEntry(Entry *entry);
  virtual bool Layout() override;
};

#endif
