#include "ArchiveExtractor.h"
#include "FileEntry.h"
#include <archive.h>
#include <archive_entry.h>
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/stdpaths.h>

Entry *ArchiveExtractor::Create(const wxFileName &filename,
                                std::function<void()> updateFnc) {
  wxFile inputFile(filename.GetFullPath());
  auto strTempDir = wxStandardPaths::Get().GetTempDir();

  wxFileName tempDirName(strTempDir + wxFileName::GetPathSeparator() +
                             "__ZPV__" + wxFileName::GetPathSeparator() +
                             filename.GetFullName(),
                         "");

  if (tempDirName.DirExists())
    tempDirName.Rmdir(wxPATH_RMDIR_RECURSIVE);

  if (!tempDirName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
    return nullptr;

  archive_entry *entry;
  int r;

  auto a = archive_read_new();
  archive_read_support_format_all(a);

  if ((r = archive_read_open_fd(a, inputFile.fd(), 10240)))
    return nullptr;

  while ((r = archive_read_next_header(a, &entry)) != ARCHIVE_EOF) {
    updateFnc();
    if (archive_entry_size(entry) < 0) {
      continue;
    }
    auto entryPathName = archive_entry_pathname(entry);

    auto entryType = archive_entry_filetype(entry);
    if (entryType != AE_IFREG && entryType != AE_IFDIR)
      continue;

    wxFileName entryFilename(entryPathName);
    wxFileName entryInTemp(tempDirName.GetFullPath() +
                           entryFilename.GetFullPath());
    entryInTemp.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    if (entryType == AE_IFREG) {
      wxFile targetFile(entryInTemp.GetFullPath(), wxFile::write);
      auto result = archive_read_data_into_fd(a, targetFile.fd());
      targetFile.Close();

      if (result < ARCHIVE_WARN)
        wxRemoveFile(entryInTemp.GetFullPath());
    }
  }
  archive_read_close(a);
  archive_read_free(a);

  return FileEntry::Create(tempDirName, updateFnc);
}
