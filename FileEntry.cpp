#include "FileEntry.h"
#include <wx/wfstream.h>

FileEntry *FileEntry::Create(const wxFileName &filename, std::function<void()> updateFnc) {
  wxArrayString paths;

  auto root = new FileEntry(filename);
  wxDir::GetAllFiles(filename.GetPath(), &paths);
  auto cmp = [](const wxFileName &f1, const wxFileName &f2) {
    return f1.GetFullPath() < f2.GetFullPath();
  };
  EntryMap entryMap(cmp);
  entryMap[filename] = root;

  for (int i = 0; i < paths.Count(); i++) {
    AddChildrenFromPath(entryMap, paths.Item(i));
    updateFnc();
  }

  root->SortChildren();
  return root;
}

wxImage FileEntry::LoadImage() {
  mutex->Lock();
  if (IsDirectory())
    return wxImage();

  wxFileInputStream stream(filename.GetFullPath());
  wxImage output(stream);
  mutex->Unlock();

  return output;
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

void FileEntry::WriteStream(wxOutputStream &output) {
  mutex->Lock();
  if (IsDirectory())
    return;

  wxFileInputStream stream(filename.GetFullPath());
  output.Write(stream);

  mutex->Unlock();
}
