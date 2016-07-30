//
// Created by noom on 10/29/15.
//

#include <algorithm>
#include <iostream>
#include <map>
#include <wx/file.h>
#include <wx/mstream.h>

#include "ZipEntry.h"

ZipEntry::ZipEntry(zip_t *zipFile, wxMutex *mutex, const wxString &innerPath)
    : zipFile(zipFile), mutex(mutex), innerPath(innerPath) {
  if (innerPath == "") {
    SetIsDirectory(true);
    SetName("/");
    return;
  }

  wxString name = innerPath;
  if (name.EndsWith("/")) {
    name = innerPath.BeforeLast('/');
    SetIsDirectory(true);
  } else {
    SetIsDirectory(false);
  }
  if (name.Contains("/"))
    name = name.AfterLast('/');

  SetName(name);
}

wxInputStream *ZipEntry::GetInputStream() {
  mutex->Lock();
  if (IsDirectory() || !zipFile)
    return nullptr;

  if(!buffer) {
    struct zip_stat stat;
    zip_stat(zipFile, innerPath, 0, &stat);

    auto file = zip_fopen(zipFile, innerPath, 0);
    size = stat.size;

    buffer = new unsigned char[size];
    auto read = zip_fread(file, buffer, size);
    zip_fclose(file);
  }
  auto output = new wxMemoryInputStream(buffer, size);

  mutex->Unlock();
  return output;
}

ZipEntry::~ZipEntry() {
  if (IsRoot()) {
    mutex->Lock();
    zip_close(zipFile);
    zipFile = nullptr;
    mutex->Unlock();
    delete mutex;
  }
  delete[] buffer;
}

ZipEntry *ZipEntry::Create(const wxFileName &filename,
                           std::function<void()> updateFnc) {
  wxFile file(filename.GetFullPath());

  int error;
  auto zipFile = zip_fdopen(file.fd(), ZIP_RDONLY, &error);
  file.Detach();
  if (zipFile == nullptr) {
    throw error;
  }
  auto mutex = new wxMutex();
  ZipEntry *root = new ZipEntry(zipFile, mutex, "");

  std::vector<wxString> innerPaths;

  int num_entries = zip_get_num_entries(zipFile, ZIP_FL_UNCHANGED);
  for (int i = 0; i < num_entries; i++) {
    auto path = wxString(zip_get_name(zipFile, i, ZIP_FL_UNCHANGED));
    innerPaths.push_back(path);
  }

  std::sort(innerPaths.begin(), innerPaths.end());
  std::map<wxString, ZipEntry *> entryMap;

  entryMap[""] = root;
  for (auto &path : innerPaths) {
    updateFnc();
    AddChildrenFromPath(zipFile, mutex, entryMap, path);
  }

  root->SortChildren();
  return root;
}

ZipEntry *
ZipEntry::AddChildrenFromPath(zip_t *zipFile, wxMutex *mutex,
                              std::map<wxString, ZipEntry *> &entryMap,
                              const wxString &innerPath) {
  auto iter = entryMap.find(innerPath);
  if (iter != entryMap.end())
    return iter->second;

  auto path = innerPath;
  bool dir = false;
  if (path.EndsWith("/")) {
    dir = true;
    path = path.BeforeLast('/');
  }

  auto parentPath = path.BeforeLast('/');
  if (parentPath != "")
    parentPath += "/";

  auto parent = AddChildrenFromPath(zipFile, mutex, entryMap, parentPath);
  auto child = new ZipEntry(zipFile, mutex, innerPath);

  parent->AddChild(child);
  entryMap[innerPath] = child;

  return child;
}
