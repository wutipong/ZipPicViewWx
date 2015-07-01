#include "ZipPicViewApp.h"
#include <wx/image.h>
#include "MainFrame.h"

wxIMPLEMENT_APP(ZipPicViewApp);

bool ZipPicViewApp::OnInit() {
  wxInitAllImageHandlers();
  auto frame = new MainFrame();
  frame->Show(true);

  return true;
}
