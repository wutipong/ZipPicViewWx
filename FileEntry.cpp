#include "FileEntry.h"
#include <wx/wfstream.h>

FileEntry *FileEntry::Create(const wxFileName &filename) {
  wxArrayString paths;
  auto root = new FileEntry(filename);
  wxDir::GetAllFiles(filename.GetPath(), &paths);
  std::map<wxString, FileEntry *> entryMap;
  entryMap[filename.GetPathWithSep()] = root;
  for (int i = 0; i < paths.Count(); i++) {
    std::cout << paths.Item(i) << std::endl;
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
    auto dirCount = filename.GetDirCount();
    auto name = filename.GetDirs().Item(dirCount - 1);
    SetName(name);
  } else {
    SetName(filename.GetFullName());
    SetIsDirectory(false);
  }
}

FileEntry *
FileEntry::AddChildrenFromPath(std::map<wxString, FileEntry *> &entryMap,
                               const wxString &path) {
  auto iter = entryMap.find(path);
  if (iter != entryMap.end())
    return iter->second;
  std::cout << path << std::endl;
  wxFileName filename(path);

  wxString parentPath = filename.GetPathWithSep();
  if (filename.IsDir()) {
    parentPath = parentPath.BeforeLast(wxFileName::GetPathSeparator())
                     .BeforeLast(wxFileName::GetPathSeparator()) +
                 wxFileName::GetPathSeparator();
  }
  auto parent = AddChildrenFromPath(entryMap, filename.GetPathWithSep());
  auto child = new FileEntry(filename);

  parent->AddChild(child);
  entryMap[path] = child;
}
