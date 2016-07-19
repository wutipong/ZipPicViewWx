//
// Created by noom on 10/29/15.
//

#ifndef ZIPPICVIEW_ZIPENTRY_H
#define ZIPPICVIEW_ZIPENTRY_H

#include <wx/filename.h>
#include <wx/thread.h>
#include <zip.h>

#include "Entry.h"
class ZipEntry : public Entry {
public:
  ZipEntry() = delete;
  ZipEntry(const ZipEntry &) = delete;

  static ZipEntry *Create(const wxFileName &filename);

  virtual wxImage LoadImage() override;
  virtual void WriteStream(wxOutputStream &) override;

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
};

#endif // ZIPPICVIEW_ZIPENTRY_H
