#ifndef __IMAGE_VIEW_PANEL__
#define __IMAGE_VIEW_PANEL__

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/button.h>

class ImageViewPanel : public wxPanel {
public:
  ImageViewPanel(wxWindow *parent, const wxImage &image,
                 wxWindowID id = wxID_ANY,
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
  wxSpinCtrl *spinctrl;

  void OnCloseButtonClick(wxCommandEvent &event);
  void OnBtnFitSizePressed(wxCommandEvent &event);
  void OnBtnActualSizePressed(wxCommandEvent &event);
  void OnScaleValueChanged(wxSpinEvent &event);

  void ResizeImage(const int& percentage);
};

#endif
