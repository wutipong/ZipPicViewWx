#include "ThumbnailLoadThread.h"

#include "MainFrame.h"

wxThread::ExitCode ThumbnailLoadThread::Entry() {
  for (auto i = 0; i < entries.size(); ++i) {
    if (TestDestroy())
      break;
    auto entry = entries[i];
    auto image = entry->LoadImage();

    auto longerSide = image.GetWidth() > image.GetHeight() ? image.GetWidth()
                                                           : image.GetHeight();
    auto width = 200 * image.GetWidth() / longerSide;
    auto height = 200 * image.GetHeight() / longerSide;
    auto thumbnailImage = image.Scale(width, height, wxIMAGE_QUALITY_HIGH);

    auto event = new wxThreadEvent(wxEVT_COMMAND_THMBTREAD_UPDATE);

    ThumbnailData data;
    data.index = i;
    data.image = thumbnailImage;
    event->SetPayload(data);

    wxQueueEvent(m_pHandler, event);
  }

  wxQueueEvent(m_pHandler, new wxThreadEvent(wxEVT_COMMAND_THMBTREAD_DONE));
  return (wxThread::ExitCode)0;
}

wxDEFINE_EVENT(wxEVT_COMMAND_THMBTREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_THMBTREAD_DONE, wxThreadEvent);
