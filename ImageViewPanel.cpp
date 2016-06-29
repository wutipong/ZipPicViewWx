#include "ImageViewPanel.h"

#include <algorithm>
#include <wx/notebook.h>
#include <wx/statline.h>

ImageViewPanel::ImageViewPanel(wxWindow *parent, Entry *entry, wxWindowID id,
                               const wxPoint &pos, const wxSize &size,
                               long style, const wxString &name)
    : wxPanel(parent, id, pos, size, style, name), timer(this) {
  auto outerSizer = new wxBoxSizer(wxVERTICAL);
  auto btnSizer = new wxBoxSizer(wxHORIZONTAL);
  btnClose = new wxButton(this, wxID_CLOSE, "Close", wxDefaultPosition,
                          wxDefaultSize, wxBU_EXACTFIT | wxBU_NOTEXT);
  btnClose->SetBitmap(wxICON(IDI_ICON_CLOSE));
  btnClose->Bind(wxEVT_BUTTON, &ImageViewPanel::OnCloseButtonClick, this);

  spnScale = new wxSpinCtrl(this, wxID_ANY);
  spnScale->SetValue(100);
  spnScale->SetRange(1, 200);
  spnScale->Bind(wxEVT_SPINCTRL, &ImageViewPanel::OnScaleValueChanged, this);

  btnFitSize = new wxToggleButton(this, wxID_ANY, "Fit", wxDefaultPosition,
                                  wxDefaultSize, wxBU_EXACTFIT | wxBU_NOTEXT);
  btnFitSize->SetBitmap(wxICON(IDI_ICON_FIT));

  btnFitSize->Bind(wxEVT_TOGGLEBUTTON, &ImageViewPanel::OnBtnFitSizeToggle,
                   this);
  btnFitSize->SetValue(true);

  btnActualSize = new wxButton(this, wxID_ANY, "Actual", wxDefaultPosition,
                               wxDefaultSize, wxBU_EXACTFIT | wxBU_NOTEXT);
  btnActualSize->Bind(wxEVT_BUTTON, &ImageViewPanel::OnBtnActualSizePressed,
                      this);
  btnActualSize->SetBitmap(wxICON(IDI_ICON_ACTUAL));
  btnActualSize->Disable();
  spnScale->Disable();
  btnSizer->Add(btnClose, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
  btnSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
                                 wxDefaultSize, wxLI_VERTICAL),
                0, wxRIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);
  btnSizer->Add(spnScale, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
  btnSizer->Add(new wxStaticText(this, wxID_ANY, "%", wxDefaultPosition,
                                 wxDefaultSize, wxST_NO_AUTORESIZE),
                wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
  btnSizer->Add(btnFitSize, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
  btnSizer->Add(btnActualSize, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
  btnSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
                                 wxDefaultSize, wxLI_VERTICAL),
                0, wxRIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

  btnNext = new wxButton(this, wxID_ANY, "Next", wxDefaultPosition,
                         wxDefaultSize, wxBU_EXACTFIT | wxBU_NOTEXT);
  btnNext->Bind(wxEVT_BUTTON, &ImageViewPanel::OnNextButtonClick, this);
  btnNext->SetBitmap(wxICON(IDI_ICON_NEXT));
  btnPrev = new wxButton(this, wxID_ANY, "Prev", wxDefaultPosition,
                         wxDefaultSize, wxBU_EXACTFIT | wxBU_NOTEXT);
  btnPrev->Bind(wxEVT_BUTTON, &ImageViewPanel::OnPrevButtonClick, this);
  btnPrev->SetBitmap(wxICON(IDI_ICON_PREV));
  btnAuto = new wxToggleButton(this, wxID_ANY, "Auto", wxDefaultPosition,
                               wxDefaultSize, wxBU_EXACTFIT | wxBU_NOTEXT);
  btnAuto->Bind(wxEVT_TOGGLEBUTTON, &ImageViewPanel::OnBtnAutoToggle, this);
  btnAuto->SetBitmap(wxICON(IDI_ICON_AUTO));
  spnRefreshTime = new wxSpinCtrl(this, wxID_ANY);
  spnRefreshTime->SetValue(30);
  spnRefreshTime->SetRange(1, 3000);

  btnSizer->Add(btnPrev, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
  btnSizer->Add(btnNext, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
  btnSizer->Add(btnAuto, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
  btnSizer->Add(spnRefreshTime, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL);
  btnSizer->Add(new wxStaticText(this, wxID_ANY, "s", wxDefaultPosition,
                                 wxDefaultSize, wxST_NO_AUTORESIZE),
                0, wxRIGHT | wxALIGN_CENTER_VERTICAL);

  outerSizer->Add(btnSizer, 0, wxALL, 10);
  outerSizer->Add(new wxStaticLine(this), 0,
                  wxRIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND, 10);
  scrollPanel = new wxScrolledWindow(this, wxID_ANY);
  scrollSizer = new wxBoxSizer(wxVERTICAL);

  scrollPanel->SetSizerAndFit(scrollSizer);
  scrollPanel->SetScrollRate(10, 10);

  outerSizer->Add(scrollPanel, 1, wxEXPAND | wxALL, 10);
  SetSizer(outerSizer);

  auto entryParent = entry->Parent();
  for (int i = 0; i < entryParent->Count(); ++i) {
    Entry *childEntry = (*entryParent)[i];

    if (childEntry->IsDirectory())
      continue;
    auto ext = childEntry->Name().AfterLast('.').Lower();

    if (ext != "jpg" && ext != "jpeg" && ext != "png" && ext != "gif")
      continue;

    entries.push_back(childEntry);
  }

  std::sort(entries.begin(), entries.end(),
            [](Entry *e1, Entry *e2) { return e1->Name() < e2->Name(); });

  entryIter = std::find(entries.begin(), entries.end(), entry);

  image = (*entryIter)->LoadImage();

  bitmap = new wxStaticBitmap(scrollPanel, wxID_ANY, wxBitmap(image));
  scrollSizer->Add(bitmap);

  Bind(wxEVT_TIMER, &ImageViewPanel::OnTimerNotify, this);
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
  RefreshImage(event.GetPosition());
}

void ImageViewPanel::OnBtnActualSizePressed(wxCommandEvent &event) {
  spnScale->SetValue(100);
  RefreshImage();
}

void ImageViewPanel::OnBtnFitSizeToggle(wxCommandEvent &event) {
  if (btnFitSize->GetValue()) {
    FitImage();
    btnActualSize->Disable();
    spnScale->Disable();
  } else {
    btnActualSize->Enable();
    spnScale->Enable();
  }
}

void ImageViewPanel::FitImage() {
  auto ctrlSize = scrollPanel->GetSize();
  auto imageSize = image.GetSize();

  auto widthPercent = (ctrlSize.GetWidth() * 100) / imageSize.GetWidth();
  auto heightPercent = (ctrlSize.GetHeight() * 100) / imageSize.GetHeight();

  auto &percentage =
      widthPercent < heightPercent ? widthPercent : heightPercent;
  percentage = percentage > 200 ? 200 : percentage;
  spnScale->SetValue(percentage);
  RefreshImage(percentage);
}

void ImageViewPanel::RefreshImage(const int &percentage) {
  if (percentage == 100) {
    bitmap->SetBitmap(wxBitmap(image));
  } else {
    int width = (image.GetWidth() * percentage) / 100;
    int height = (image.GetHeight() * percentage) / 100;

    bitmap->SetBitmap(
        wxBitmap(image.Scale(width, height, wxIMAGE_QUALITY_HIGH)));
  }
  scrollSizer->FitInside(scrollPanel);
  spnScale->SetValue(percentage);
  Refresh();
  Update();
}

void ImageViewPanel::OnNextButtonClick(wxCommandEvent &event) { Advance(1); }

void ImageViewPanel::OnPrevButtonClick(wxCommandEvent &event) { Advance(-1); }

void ImageViewPanel::SetImageEntry(Entry *entry) {
  auto notebook = dynamic_cast<wxNotebook *>(GetParent());
  if (!notebook)
    return;
  auto currentPage = notebook->GetSelection();
  if (currentPage == wxNOT_FOUND)
    return;
  notebook->SetPageText(currentPage, entry->Name());

  image = entry->LoadImage();
  if (btnFitSize->GetValue())
    FitImage();
  else
    RefreshImage();
}

bool ImageViewPanel::Layout() {
  wxPanel::Layout();
  if (btnFitSize->GetValue())
    FitImage();
  return false;
}

void ImageViewPanel::Advance(const int &step) {
  if (entryIter + step >= entries.end())
    entryIter = entries.begin();
  else if (entryIter + step < entries.begin())
    entryIter = entries.end() - 1;
  else
    entryIter = entryIter + step;

  SetImageEntry(*entryIter);
}

void ImageViewPanel::OnBtnAutoToggle(wxCommandEvent &event) {
  if (btnAuto->GetValue()) {
    timer.Start(spnRefreshTime->GetValue() * 1000);
    spnRefreshTime->Disable();
    btnPrev->Disable();
    btnNext->Disable();
  } else {
    timer.Stop();
    spnRefreshTime->Enable();
    btnPrev->Enable();
    btnNext->Enable();
  }
}

void ImageViewPanel::OnTimerNotify(wxTimerEvent &timer) { Advance(1); }
