#ifndef __THUMBNAIL_PANEL_H__
#define __THUMBNAIL_PANEL_H__
#include <wx/wx.h>
#include <memory>

class ThumbnailPanel : public wxControl {
public:
  ThumbnailPanel(const wxString &label, const wxImage &image);

private:
  wxStaticText *textCtl;
  wxStaticBitmap *bitmapCtl;

  // wxDECLARE_DYNAMIC_CLASS(ThumbnailPanel);
  // wxDECLARE_EVENT_TABLE();
};

#endif
