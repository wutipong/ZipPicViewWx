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

wxImage ZipEntry::LoadImage() {
  mutex->Lock();
  if (IsDirectory())
    return wxImage();

  struct zip_stat stat;
  zip_stat(zipFile, innerPath, 0, &stat);

  auto file = zip_fopen(zipFile, innerPath, 0);
  auto size = stat.size;
  auto buffer = new unsigned char[size];
  auto read = zip_fread(file, buffer, size);

  wxMemoryInputStream stream(buffer, size);
  wxImage output(stream);

  delete[] buffer;
  zip_fclose(file);
  mutex->Unlock();
  return output;
}

ZipEntry::~ZipEntry() {
  if (IsRoot()) {
    zip_close(zipFile);
    delete mutex;
  }
}

ZipEntry *ZipEntry::Create(const wxString &path) {
  wxFile file(path);

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

void ZipEntry::WriteStream(wxOutputStream &output) {
  mutex->Lock();
  if (IsDirectory())
    return;

  struct zip_stat stat;
  zip_stat(zipFile, innerPath, 0, &stat);

  auto file = zip_fopen(zipFile, innerPath, 0);
  auto size = stat.size;
  auto buffer = new unsigned char[size];
  auto read = zip_fread(file, buffer, size);

  wxMemoryInputStream stream(buffer, size);
  output.Write(stream);

  delete[] buffer;
  zip_fclose(file);
  mutex->Unlock();
}
