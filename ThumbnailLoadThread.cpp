#include "ThumbnailLoadThread.h"

#include "MainFrame.h"
#include <wx/log.h>

wxThread::ExitCode ThumbnailLoadThread::Entry() {
  bool cancelled = false;
  for (auto i = 0; i < entries.size(); ++i) {

    if (TestDestroy()) {
      cancelled = true;
      break;
    }

    auto entry = entries[i];
    auto image = entry->CreateImage();

    auto longerSide = image.GetWidth() > image.GetHeight() ? image.GetWidth()
                                                           : image.GetHeight();
    auto width = 200 * image.GetWidth() / longerSide;
    auto height = 200 * image.GetHeight() / longerSide;
    auto thumbnailImage = image.Scale(width, height, wxIMAGE_QUALITY_HIGH);

    auto event = new wxThreadEvent(wxEVT_COMMAND_THMBTREAD_UPDATE);

    ThumbnailData data;
    data.id = GetId();
    data.index = i;
    data.image = thumbnailImage;
    data.total = entries.size();
    event->SetPayload(data);
    wxQueueEvent(m_pHandler, event);
  }

  auto event = new wxThreadEvent(wxEVT_COMMAND_THMBTREAD_DONE);
  event->SetExtraLong(GetId());
  wxQueueEvent(m_pHandler, event);
  return (wxThread::ExitCode) new bool(cancelled);
}

wxDEFINE_EVENT(wxEVT_COMMAND_THMBTREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_THMBTREAD_DONE, wxThreadEvent);
