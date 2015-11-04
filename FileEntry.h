#ifndef ZIPPICVIEW_FILEENTRY_H
#define ZIPPICVIEW_FILEENTRY_H

#include <wx/filename.h>
#include <wx/dir.h>
#include <map>
#include <functional>
#include "Entry.h"

class FileEntry : public Entry {
public:
  FileEntry() = delete;
  FileEntry(const FileEntry &) = delete;

  static FileEntry *Create(const wxFileName &filename);

  virtual wxImage LoadImage() override;

private:
  FileEntry(const wxFileName &filename);
  wxFileName filename;

  static FileEntry *AddChildrenFromPath(
      std::map<wxFileName, FileEntry *,
               std::function<bool(const wxFileName &, const wxFileName &)>>
          &entryMap,
      const wxFileName &filename);
};

#endif
