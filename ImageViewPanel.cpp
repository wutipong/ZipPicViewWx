#include "ImageViewPanel.h"

#include <wx/notebook.h>
#include <wx/artprov.h>

ImageViewPanel::ImageViewPanel(wxWindow *parent, const wxImage &image,
                               wxWindowID id, const wxPoint &pos,
                               const wxSize &size, long style,
                               const wxString &name)
    : wxPanel(parent, id, pos, size, style, name), image(image) {
  auto outerSizer = new wxBoxSizer(wxVERTICAL);
  auto btnSizer = new wxBoxSizer(wxHORIZONTAL);
  btnClose = new wxButton(this, wxID_CLOSE, "Close");

  auto icon = wxArtProvider::GetBitmap(wxART_CLOSE, wxART_BUTTON);
  btnClose->SetBitmap(icon);
  btnClose->Bind(wxEVT_BUTTON, &ImageViewPanel::OnCloseButtonClick, this);

  spinctrl = new wxSpinCtrl(this, wxID_ANY);
  spinctrl->SetValue(100);
  spinctrl->SetRange(1, 200);
  spinctrl->Bind(wxEVT_SPINCTRL, &ImageViewPanel::OnScaleValueChanged, this);

  auto btnFitSize = new wxButton(this, wxID_ANY, "Fit");
  btnFitSize->Bind(wxEVT_BUTTON, &ImageViewPanel::OnBtnFitSizePressed, this);

  auto btnActualSize = new wxButton(this, wxID_ANY, "Actual Size");
  btnActualSize->Bind(
    wxEVT_BUTTON, &ImageViewPanel::OnBtnActualSizePressed, this);

  btnSizer->Add(btnClose, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(spinctrl, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(btnFitSize, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(btnActualSize, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

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
  ResizeImage(event.GetPosition());
}

void ImageViewPanel::OnBtnActualSizePressed(wxCommandEvent &event) {
  spinctrl->SetValue(100);
  ResizeImage(100);
}

void ImageViewPanel::OnBtnFitSizePressed(wxCommandEvent &event) {
  auto ctrlSize = scrollPanel->GetSize();
  auto imageSize = image.GetSize();

  auto widthPercent = (ctrlSize.GetWidth() * 100) / imageSize.GetWidth();
  auto heightPercent = (ctrlSize.GetHeight() * 100) / imageSize.GetHeight();

  auto& percentage = widthPercent < heightPercent ?
                     widthPercent : heightPercent;

  spinctrl->SetValue(percentage);
  ResizeImage(percentage);
}

void ImageViewPanel::ResizeImage(const int& percentage) {
  if (percentage == 100) {
    bitmap->SetBitmap(wxBitmap(image));
    return;
  }
  int width = (image.GetWidth() * percentage) / 100;
  int height = (image.GetHeight() * percentage) / 100;

  bitmap->SetBitmap(wxBitmap(image.Scale(width, height, wxIMAGE_QUALITY_HIGH )));

  scrollSizer->FitInside(scrollPanel);
  Refresh();
  Update();
}
