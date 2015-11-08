#include "ZipPicViewApp.h"
#include <wx/image.h>

#include "MainFrame.h"

wxIMPLEMENT_APP(ZipPicViewApp);

bool ZipPicViewApp::OnInit() {
  wxInitAllImageHandlers();
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
