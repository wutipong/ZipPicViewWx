#include "ZipPicViewApp.h"
#include "MainFrame.h"
#include <Magick++.h>
#include <wx/image.h>

wxIMPLEMENT_APP(ZipPicViewApp);

bool ZipPicViewApp::OnInit() {
  wxInitAllImageHandlers();
  Magick::InitializeMagick(nullptr);
  log = new wxLogStderr;
  wxLog::SetActiveTarget(log);
  auto frame = new MainFrame();
  frame->Show(true);

  return true;
}

int ZipPicViewApp::OnExit() {
  wxApp::OnExit();
  wxLog::SetActiveTarget(nullptr);
  delete log;

  return 0;
}
