#include "ThumbnailPanel.h"

ThumbnailPanel::ThumbnailPanel(wxWindow *parent, wxWindowID id,
                               const wxString &label, const wxImage &image,
                               const wxPoint &pos, const wxSize &size,
                               long style, const wxString &name)
    : wxWindow(parent, id, pos, size, style, name) {
  auto sizer = new wxBoxSizer(wxVERTICAL);
  auto thumbnailImage = image.Scale(200, 200);
  bitmapCtl = new wxStaticBitmap(this, wxID_ANY, wxBitmap(thumbnailImage));
  textCtl = new wxStaticText(this, wxID_ANY, label);
  sizer->Add(bitmapCtl, 0, wxALIGN_CENTER);
  sizer->Add(textCtl, 0, wxALIGN_CENTER);

  SetSizer(sizer);

  SetMaxSize({250, 250});
  SetMinSize({250, 250});
}
