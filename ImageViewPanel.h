#ifndef __IMAGE_VIEW_PANEL__
#define __IMAGE_VIEW_PANEL__

#include <wx/wx.h>

class ImageViewPanel : public wxPanel {
public:
  ImageViewPanel(wxWindow *parent, const wxImage &image, const size_t page,
                 wxWindowID id = wxID_ANY,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = wxTAB_TRAVERSAL,
                 const wxString &name = "ImageViewPanel");

private:
  size_t page;
  wxButton *btnClose;
  void OnCloseButtonClick(wxCommandEvent &event);
  wxDECLARE_EVENT_TABLE();
};

#endif
