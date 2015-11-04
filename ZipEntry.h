//
// Created by noom on 10/29/15.
//

#ifndef ZIPPICVIEW_ZIPENTRY_H
#define ZIPPICVIEW_ZIPENTRY_H

#include <wx/file.h>
#include <zip.h>
#include "Entry.h"
class ZipEntry : public Entry {
public:
  ZipEntry() = delete;
  ZipEntry(const ZipEntry &) = delete;

  static ZipEntry *Create(const wxString &path);

  virtual wxImage LoadImage() override;

  virtual ~ZipEntry();

private:
  static ZipEntry *AddChildrenFromPath(wxFile *file, zip_t *zipFile,
                                       std::map<wxString, ZipEntry *> &entryMap,
                                       const wxString &innerPath);

  ZipEntry(wxFile *file, zip_t *zipFile, const wxString &innerPath);
  zip_t *zipFile;
  wxFile *file;
  wxString innerPath;
};

#endif // ZIPPICVIEW_ZIPENTRY_H
