#include "ImageViewPanel.h"
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/artprov.h>
#include <wx/spinctrl.h>

ImageViewPanel::ImageViewPanel(wxWindow *parent, const wxImage &image,
                               wxWindowID id, const wxPoint &pos,
                               const wxSize &size, long style,
                               const wxString &name)
    : wxPanel(parent, id, pos, size, style, name), image(image) {
  auto outerSizer = new wxBoxSizer(wxVERTICAL);
  auto *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  btnClose = new wxButton(this, wxID_CLOSE, "Close");

  auto icon = wxArtProvider::GetBitmap(wxART_CLOSE, wxART_BUTTON);
  btnClose->SetBitmap(icon);
  btnClose->Bind(wxEVT_BUTTON, &ImageViewPanel::OnCloseButtonClick, this);

  auto spinctrl = new wxSpinCtrl(this, wxID_ANY);
  spinctrl->SetValue(100);
  spinctrl->SetRange(1, 200);
  spinctrl->Bind(wxEVT_SPINCTRL, &ImageViewPanel::OnScaleValueChanged, this);

  btnSizer->Add(btnClose, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(spinctrl, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

  outerSizer->Add(btnSizer, 0, wxALL, 10);

  scrollPanel = new wxScrolledWindow(this, wxID_ANY);
  bitmap = new wxStaticBitmap(scrollPanel, wxID_ANY, wxBitmap(image));
  scrollSizer = new wxBoxSizer(wxVERTICAL);
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

void ImageViewPanel::OnScaleValueChanged(wxSpinEvent &event) {
  if (event.GetPosition() == 100) {
    bitmap->SetBitmap(wxBitmap(image));
    return;
  }
  int width = (image.GetWidth() * event.GetPosition()) / 100;
  int height = (image.GetHeight() * event.GetPosition()) / 100;

  bitmap->SetBitmap(wxBitmap(image.Scale(width, height)));

  scrollSizer->FitInside(scrollPanel);
}
