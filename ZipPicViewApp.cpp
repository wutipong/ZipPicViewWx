#include "ZipPicViewApp.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(ZipPicViewApp);

bool ZipPicViewApp::OnInit() {
  auto frame = new MainFrame();
  frame->Show(true);

  return true;
}
