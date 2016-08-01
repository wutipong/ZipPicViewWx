#ifndef ZIPPICVIEW_FILEENTRY_H
#define ZIPPICVIEW_FILEENTRY_H

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/thread.h>

#include "Entry.h"
#include <functional>
#include <map>

class FileEntry : public Entry {
public:
  typedef std::map<wxFileName, FileEntry *,
                   std::function<bool(const wxFileName &, const wxFileName &)>>
      EntryMap;

  FileEntry() = delete;
  FileEntry(const FileEntry &) = delete;
  virtual ~FileEntry();
  static FileEntry *Create(const wxFileName &filename,
                           std::function<bool()> updateFnc);

  virtual bool IsRoot() const override { return isRoot; }

private:
  FileEntry(const wxFileName &filename, const bool &isRoot = false);
  wxFileName filename;
  wxMutex *mutex;
  bool isRoot{false};

  static FileEntry *AddChildrenFromPath(EntryMap &entryMap,
                                        const wxFileName &filename);

protected:
  virtual wxInputStream *GetInputStream() override;
};

#endif
