#include "ImageViewPanel.h"
#include <wx/button.h>
#include <wx/notebook.h>

ImageViewPanel::ImageViewPanel(wxWindow *parent, const wxImage &image,
                               wxWindowID id, const wxPoint &pos,
                               const wxSize &size, long style,
                               const wxString &name)
    : wxPanel(parent, id, pos, size, style, name) {
  auto outerSizer = new wxBoxSizer(wxVERTICAL);
  auto *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  btnClose = new wxButton(this, wxID_CLOSE, "Close");
  btnClose->Bind(wxEVT_BUTTON, &ImageViewPanel::OnCloseButtonClick, this);
  btnSizer->Add(btnClose);
  outerSizer->Add(btnSizer, 0, wxALL, 10);

  wxScrolledWindow *scrollPanel = new wxScrolledWindow(this, wxID_ANY);
  wxStaticBitmap *bitmap =
      new wxStaticBitmap(scrollPanel, wxID_ANY, wxBitmap(image));
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
  auto currentPage = notebook->GetSelection();
  if (currentPage == wxNOT_FOUND)
    return;
  notebook->DeletePage(currentPage);
}
