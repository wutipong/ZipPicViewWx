#ifndef THUMBNAIL_LOAD_THREAD_H
#define THUMBNAIL_LOAD_THREAD_H

#include <memory>
#include <vector>
#include <wx/thread.h>
#include <wx/wx.h>

#include "Entry.h"

wxDECLARE_EVENT(wxEVT_COMMAND_THMBTREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_THMBTREAD_DONE, wxThreadEvent);

class MainFrame;

class ThumbnailLoadThread : public wxThread {
public:
  ThumbnailLoadThread(MainFrame *handler, const std::vector<::Entry *> &entries,
                      std::shared_ptr<::Entry> pRootEntry, wxMutex &mutex)
      : wxThread(wxTHREAD_DETACHED), entries(entries), pRootEntry(pRootEntry),
        mutex(mutex) {
    m_pHandler = handler;
    ThumbnailLoadThread::entries = entries;
  };

private:
  virtual wxThread::ExitCode Entry() override;
  MainFrame *m_pHandler;
  std::vector<::Entry *> entries;
  wxMutex &mutex;
  std::shared_ptr<::Entry> pRootEntry;
};

struct ThumbnailData {
  wxThreadIdType id;
  int index;
  int total;
  wxImage image;
};

#endif
