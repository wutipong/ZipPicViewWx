#include "Entry.h"
#include <wx/icon.h>
#include "res/img_error.xpm"

wxImage Entry::CreateImage() {
  if (IsDirectory())
    return wxImage();

  auto stream = GetInputStream();
  if(!stream) return wxImage(IMG_ERROR_xpm);
  wxImage output(*stream);
  delete stream;

  if(!output.IsOk())
    return wxImage(IMG_ERROR_xpm);

  return output;
}

void Entry::WriteStream(wxOutputStream &output) {
  auto stream = GetInputStream();
  if(!stream) return;
  output.Write(*stream);
  delete stream;
}

Entry::~Entry() {
  for (auto entry : children) {
    delete entry;
  }
  children.clear();
}

void Entry::PrintChildren(const int &level) {
  for (int i = 0; i < level; i++)
    std::cout << "|-";
  std::cout << Name() << std::endl;
  for (auto child : children) {
    child->PrintChildren(level + 1);
  }
}

void Entry::SortChildren() {
  std::sort(children.begin(), children.end(), [](Entry *e1, Entry *e2) {
    if (e1->IsDirectory() == e2->IsDirectory()) {
      return e1->Name() < e2->Name();
    } else if (e1->IsDirectory()) {
      return true;
    } else {
      return false;
    }
  });

  std::for_each(children.begin(), children.end(),
                [](Entry *e) { e->SortChildren(); });
}
