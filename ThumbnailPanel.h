#ifndef __THUMBNAIL_PANEL_H__
#define __THUMBNAIL_PANEL_H__
#include <wx/wx.h>
#include <memory>

class ThumbnailPanel : public wxWindow {
public:
  ThumbnailPanel(wxWindow *parent, wxWindowID id, const wxString &label,
                 const wxImage &image, const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize, long style = 0,
                 const wxString &name = "ThumbnailPanel");

private:
  wxStaticText *textCtl;
  wxStaticBitmap *bitmapCtl;

  // wxDECLARE_DYNAMIC_CLASS(ThumbnailPanel);
  // wxDECLARE_EVENT_TABLE();
};

#endif
