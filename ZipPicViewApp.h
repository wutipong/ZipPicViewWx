#ifndef __ZIP_PIC_VIEW_APP_H__
#define __ZIP_PIC_VIEW_APP_H__

#include <wx/wx.h>
#include <wx/log.h>

class ZipPicViewApp : public wxApp {
public:
  virtual bool OnInit();
  virtual int OnExit();

private:
  wxLog *log;
};

#endif
