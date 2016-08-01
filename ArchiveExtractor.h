#ifndef ARCHIVE_EXTRACTOR_H
#define ARCHIVE_EXTRACTOR_H

#include "Entry.h"
#include <functional>
#include <wx/filename.h>

class ArchiveExtractor {
public:
  static Entry *Create(const wxFileName &filename,
                       std::function<bool()> updateFnc);
};

#endif
