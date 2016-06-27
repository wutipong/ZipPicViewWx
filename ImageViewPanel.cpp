#include "ImageViewPanel.h"

#include <algorithm>
#include <wx/notebook.h>

ImageViewPanel::ImageViewPanel(wxWindow *parent, Entry *entry, wxWindowID id,
                               const wxPoint &pos, const wxSize &size,
                               long style, const wxString &name)
    : wxPanel(parent, id, pos, size, style, name) {
  auto outerSizer = new wxBoxSizer(wxVERTICAL);
  auto btnSizer = new wxBoxSizer(wxHORIZONTAL);
  btnClose = new wxButton(this, wxID_CLOSE, "Close");
  btnClose->Bind(wxEVT_BUTTON, &ImageViewPanel::OnCloseButtonClick, this);

  spnScale = new wxSpinCtrl(this, wxID_ANY);
  spnScale->SetValue(100);
  spnScale->SetRange(1, 200);
  spnScale->Bind(wxEVT_SPINCTRL, &ImageViewPanel::OnScaleValueChanged, this);

  auto btnFitSize = new wxButton(this, wxID_ANY, "Fit");
  btnFitSize->Bind(wxEVT_BUTTON, &ImageViewPanel::OnBtnFitSizePressed, this);

  auto btnActualSize = new wxButton(this, wxID_ANY, "Actual Size");
  btnActualSize->Bind(wxEVT_BUTTON, &ImageViewPanel::OnBtnActualSizePressed,
                      this);

  btnSizer->Add(btnClose, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(spnScale, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(btnFitSize, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(btnActualSize, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

  btnNext = new wxButton(this, wxID_ANY, "Next");
  btnNext->Bind(wxEVT_BUTTON, &ImageViewPanel::OnNextButtonClick, this);
  btnPrev = new wxButton(this, wxID_ANY, "Prev");
  btnPrev->Bind(wxEVT_BUTTON, &ImageViewPanel::OnPrevButtonClick, this);
  btnAuto = new wxToggleButton(this, wxID_ANY, "Auto");
  spnRefreshTime = new wxSpinCtrl(this, wxID_ANY);
  spnRefreshTime->SetValue(30);
  spnRefreshTime->SetRange(1, 3000);

  btnSizer->Add(btnPrev, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(btnNext, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(btnAuto, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
  btnSizer->Add(spnRefreshTime, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

  outerSizer->Add(btnSizer, 0, wxALL, 10);

  scrollPanel = new wxScrolledWindow(this, wxID_ANY);
  scrollSizer = new wxBoxSizer(wxVERTICAL);

  scrollPanel->SetSizerAndFit(scrollSizer);
  scrollPanel->SetScrollRate(10, 10);

  outerSizer->Add(scrollPanel, 1, wxEXPAND | wxALL, 10);
  SetSizer(outerSizer);

  entryParent = entry->Parent();
  entryIter = std::find(entryParent->begin(), entryParent->end(), entry);
  image = (*entryIter)->LoadImage();

  bitmap = new wxStaticBitmap(scrollPanel, wxID_ANY, wxBitmap(image));
  scrollSizer->Add(bitmap);
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
  spnScale->SetValue(100);
  ResizeImage(100);
}

void ImageViewPanel::OnBtnFitSizePressed(wxCommandEvent &event) { LoadImage(); }

void ImageViewPanel::LoadImage() {
  auto ctrlSize = scrollPanel->GetSize();
  auto imageSize = image.GetSize();

  auto widthPercent = (ctrlSize.GetWidth() * 100) / imageSize.GetWidth();
  auto heightPercent = (ctrlSize.GetHeight() * 100) / imageSize.GetHeight();

  auto &percentage =
      widthPercent < heightPercent ? widthPercent : heightPercent;

  spnScale->SetValue(percentage);
  ResizeImage(percentage);
}

void ImageViewPanel::ResizeImage(const int &percentage) {
  if (percentage == 100) {
    bitmap->SetBitmap(wxBitmap(image));
    return;
  }
  int width = (image.GetWidth() * percentage) / 100;
  int height = (image.GetHeight() * percentage) / 100;

  bitmap->SetBitmap(wxBitmap(image.Scale(width, height, wxIMAGE_QUALITY_HIGH)));

  scrollSizer->FitInside(scrollPanel);
  Refresh();
  Update();
}

void ImageViewPanel::OnNextButtonClick(wxCommandEvent &event) {
  entryIter++;
  if (entryIter == entryParent->end())
    entryIter = entryParent->begin();
  image = (*entryIter)->LoadImage();
  LoadImage();

  auto notebook = dynamic_cast<wxNotebook *>(GetParent());
  if (!notebook)
    return;
  auto currentPage = notebook->GetSelection();
  if (currentPage == wxNOT_FOUND)
    return;
  notebook->SetPageText(currentPage, (*entryIter)->Name());
}
void ImageViewPanel::OnPrevButtonClick(wxCommandEvent &event) {
  if (entryIter == entryParent->begin())
    entryIter = entryParent->end() - 1;
  else
    entryIter--;
  image = (*entryIter)->LoadImage();
  LoadImage();

  auto notebook = dynamic_cast<wxNotebook *>(GetParent());
  if (!notebook)
    return;
  auto currentPage = notebook->GetSelection();
  if (currentPage == wxNOT_FOUND)
    return;
  notebook->SetPageText(currentPage, (*entryIter)->Name());
}
