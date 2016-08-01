//
// Created by noom on 10/29/15.
//

#ifndef ZIPPICVIEW_ZIPENTRY_H
#define ZIPPICVIEW_ZIPENTRY_H

#include <functional>
#include <wx/filename.h>
#include <wx/thread.h>
#include <zip.h>

#include "Entry.h"
class ZipEntry : public Entry {
public:
  ZipEntry() = delete;
  ZipEntry(const ZipEntry &) = delete;

  static ZipEntry *Create(const wxFileName &filename,
                          std::function<bool()> updateFnc);

  virtual ~ZipEntry();
  virtual bool IsRoot() const override { return innerPath.IsEmpty(); }

private:
  static ZipEntry *AddChildrenFromPath(zip_t *zipFile, wxMutex *mutex,
                                       std::map<wxString, ZipEntry *> &entryMap,
                                       const wxString &innerPath);

  ZipEntry(zip_t *zipFile, wxMutex *mutex, const wxString &innerPath);
  zip_t *zipFile;
  wxString innerPath;
  wxMutex *mutex;
  unsigned char *buffer = nullptr;
  size_t size;

protected:
  virtual wxInputStream *GetInputStream() override;
};

#endif // ZIPPICVIEW_ZIPENTRY_H
