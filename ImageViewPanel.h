#ifndef __IMAGE_VIEW_PANEL__
#define __IMAGE_VIEW_PANEL__

#include "Entry.h"
#include <random>
#include <wx/button.h>
#include <wx/filename.h>
#include <wx/spinctrl.h>
#include <wx/tglbtn.h>
#include <wx/timer.h>
#include <wx/wx.h>

#ifdef USE_CIMG
#include "CImg/CImg.h"
#endif

class ImageViewPanel : public wxPanel {
public:
  ImageViewPanel(wxWindow *parent, Entry *entry, wxWindowID id = wxID_ANY,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = wxTAB_TRAVERSAL,
                 const wxString &name = "ImageViewPanel");

  void SwitchToTheActualImage();

private:
  wxButton *btnClose;
  wxStaticBitmap *bitmapStatic;
  wxScrolledWindow *scrollPanel;
  wxBoxSizer *scrollSizer;
  wxImage image;
  wxSpinCtrl *spnScale;
  wxButton *btnNext;
  wxButton *btnPrev;
  wxToggleButton *btnAuto;
  wxToggleButton *btnRandom;
  wxSpinCtrl *spnRefreshTime;
  wxToggleButton *btnFitSize;
  wxButton *btnActualSize;
  wxTimer timer;
  wxFileName filename;

  std::vector<Entry *> entries;
  std::vector<Entry *>::const_iterator entryIter;

  std::random_device r;
  std::default_random_engine randomEngine;
  std::uniform_int_distribution<int> random;

#ifdef USE_CIMG
  cimg_library::CImg<unsigned char> cimgImage;
  cimg_library::CImg<unsigned char> cimgAlpha;

  static cimg_library::CImg<unsigned char>
  CImgFromWxImage(const wxImage &image);
  static cimg_library::CImg<unsigned char>
  CImgAlphaFromWxImage(const wxImage &image);
  static wxImage
  wxImageFromCImg(const cimg_library::CImg<unsigned char> &img,
                  cimg_library::CImg<unsigned char> *alpha = nullptr);
#endif

  void OnCloseButtonClick(wxCommandEvent &event);
  void OnBtnFitSizeToggle(wxCommandEvent &event);
  void OnBtnAutoToggle(wxCommandEvent &event);
  void OnBtnActualSizePressed(wxCommandEvent &event);
  void OnScaleValueChanged(wxSpinEvent &event);
  void OnNextButtonClick(wxCommandEvent &event);
  void OnPrevButtonClick(wxCommandEvent &event);
  void OnSaveButtonClick(wxCommandEvent &event);

  void RefreshImage(const int &percentage = 100);

  void FitImage();
  void SetImageEntry(Entry *entry);

  void Advance(const int &step);
  virtual bool Layout() override;
  void OnTimerNotify(wxTimerEvent &event);

  wxImage CreateScaledImage(const unsigned int &width,
                            const unsigned int &height,
                            const bool isHighQuality = false);
};

#endif
