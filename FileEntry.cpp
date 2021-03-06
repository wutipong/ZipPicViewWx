#include "FileEntry.h"
#include <wx/wfstream.h>

FileEntry *FileEntry::Create(const wxFileName &filename,
                             std::function<bool()> updateFnc) {
  wxArrayString paths;

  bool cancelled = false;
  auto root = new FileEntry(filename);
  wxDir::GetAllFiles(filename.GetPath(), &paths);
  auto cmp = [](const wxFileName &f1, const wxFileName &f2) {
    return f1.GetFullPath() < f2.GetFullPath();
  };
  EntryMap entryMap(cmp);
  entryMap[filename] = root;

  for (int i = 0; i < paths.Count(); i++) {
    AddChildrenFromPath(entryMap, paths.Item(i));
    bool loadNext = updateFnc();

    if (!loadNext) {
      cancelled = true;
      break;
    }
  }

  if (cancelled) {
    delete root;
    return nullptr;
  }

  root->SortChildren();
  return root;
}

wxInputStream *FileEntry::GetInputStream() {
  if (IsDirectory())
    return nullptr;
  return new wxFileInputStream(filename.GetFullPath());
}

FileEntry::FileEntry(const wxFileName &filename, const bool &isRoot)
    : filename(filename), isRoot(isRoot) {
  mutex = new wxMutex();
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

FileEntry *FileEntry::AddChildrenFromPath(EntryMap &entryMap,
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

FileEntry::~FileEntry() { delete mutex; }
