#include "FileEntry.h"
#include <wx/wfstream.h>

FileEntry *FileEntry::Create(const wxFileName &filename) {
  wxArrayString paths;
  auto root = new FileEntry(filename);
  wxDir::GetAllFiles(filename.GetPath(), &paths);
  auto cmp = [](const wxFileName &f1, const wxFileName &f2) {
    return f1.GetFullPath() < f2.GetFullPath();
  };
  std::map<wxFileName, FileEntry *,
           std::function<bool(const wxFileName &, const wxFileName &)>>
      entryMap(cmp);
  entryMap[filename] = root;

  for (int i = 0; i < paths.Count(); i++) {
    AddChildrenFromPath(entryMap, paths.Item(i));
  }

  return root;
}

wxImage FileEntry::LoadImage() {
  if (IsDirectory())
    return wxImage();

  wxFileInputStream stream(filename.GetFullPath());
  wxImage output(stream);

  return output;
}

FileEntry::FileEntry(const wxFileName &filename) : filename(filename) {
  if (filename.IsDir()) {
    SetIsDirectory(true);
    auto count = filename.GetDirCount();
    auto name = filename.GetDirs()[count - 1];
    SetName(name);
  } else {

    SetName(filename.GetFullName());
    SetIsDirectory(false);
  }
}

FileEntry *FileEntry::AddChildrenFromPath(
    std::map<wxFileName, FileEntry *,
             std::function<bool(const wxFileName &, const wxFileName &)>>
        &entryMap,
    const wxFileName &filename) {
  auto iter = entryMap.find(filename);
  if (iter != entryMap.end()) {
    return iter->second;
  }

  wxFileName parentFileName = filename;

  if (parentFileName.IsDir()) {
    parentFileName.RemoveLastDir();
  } else {
    parentFileName.SetFullName("");
  }

  auto parent = AddChildrenFromPath(entryMap, parentFileName);
  auto child = new FileEntry(filename);

  parent->AddChild(child);
  entryMap[filename] = child;

  return child;
}
