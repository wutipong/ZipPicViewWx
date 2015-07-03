#include "ImageViewPanel.h"
#include <wx/button.h>
#include <wx/notebook.h>

ImageViewPanel::ImageViewPanel(wxWindow *parent, const wxImage &image,
                               const size_t page, wxWindowID id,
                               const wxPoint &pos, const wxSize &size,
                               long style, const wxString &name)
    : wxPanel(parent, id, pos, size, style, name), page(page) {
  auto outerSizer = new wxBoxSizer(wxVERTICAL);
  auto *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  btnClose = new wxButton(this, wxID_CLOSE, "Close");
  btnSizer->Add(btnClose);
  outerSizer->Add(btnSizer, 0, wxALL, 10);

  wxScrolledWindow *scrollPanel = new wxScrolledWindow(this, wxID_ANY);
  wxStaticBitmap *bitmap = new wxStaticBitmap(scrollPanel, wxID_ANY, wxBitmap(image));
  auto scrollSizer = new wxBoxSizer(wxVERTICAL);
  scrollSizer->Add(bitmap);
  scrollPanel->SetSizerAndFit(scrollSizer);
  scrollPanel->SetScrollRate(10, 10);

  outerSizer->Add(scrollPanel, 1, wxEXPAND | wxALL, 10);
  SetSizer(outerSizer);
}

void ImageViewPanel::OnCloseButtonClick(wxCommandEvent &event) {
  auto notebook = dynamic_cast<wxNotebook *>(GetParent());
  if (!notebook)
    return;
  notebook->DeletePage(page);
}
// clang-format off
wxBEGIN_EVENT_TABLE(ImageViewPanel, wxPanel)
  EVT_BUTTON(wxID_CLOSE, ImageViewPanel::OnCloseButtonClick)
wxEND_EVENT_TABLE();
// clang-format on
