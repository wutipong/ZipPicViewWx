#ifndef THUMBNAIL_LOAD_THREAD_H
#define THUMBNAIL_LOAD_THREAD_H

#include <vector>
#include <wx/wx.h>
#include <wx/thread.h>
#include "Entry.h"

wxDECLARE_EVENT(wxEVT_COMMAND_THMBTREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_THMBTREAD_DONE, wxThreadEvent);

class MainFrame;

class ThumbnailLoadThread : public wxThread {
public:
  ThumbnailLoadThread(MainFrame *handler, const std::vector<::Entry *> &entries)
      : wxThread(wxTHREAD_DETACHED) {
    m_pHandler = handler;
    ThumbnailLoadThread::entries = entries;
  };

private:
  virtual wxThread::ExitCode Entry() override;
  MainFrame *m_pHandler;
  std::vector<::Entry *> entries;
};

struct ThumbnailData {
  int index;
  wxImage image;
};

#endif
