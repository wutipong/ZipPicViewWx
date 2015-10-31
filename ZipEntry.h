//
// Created by noom on 10/29/15.
//

#ifndef ZIPPICVIEW_ZIPENTRY_H
#define ZIPPICVIEW_ZIPENTRY_H


#include <zip.h>
#include "Entry.h"
class ZipEntry : public Entry {
 public:
  ZipEntry() = delete;
  ZipEntry(const ZipEntry&) = delete;

  static ZipEntry *Create(zip_t *zipFile);

  virtual wxImage LoadImage() override ;

  virtual ~ZipEntry();
 private:
  static ZipEntry* AddChildrenFromPath(zip_t *zipFile, std::map<wxString, ZipEntry*>& entryMap, const wxString& innerPath);

  ZipEntry(zip_t *zipFile, const wxString& innerPath);
  zip_t *zipFile;
  wxString innerPath;
};


#endif //ZIPPICVIEW_ZIPENTRY_H
