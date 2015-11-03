//
// Created by noom on 10/29/15.
//

#ifndef ZIPPICVIEW_ENTRY_H
#define ZIPPICVIEW_ENTRY_H

#include <iostream>
#include <vector>

#include <wx/string.h>
#include <wx/image.h>
#include <wx/treebase.h>

class Entry;
typedef std::vector<Entry *>::const_iterator EntryIter;

class Entry {
public:
  wxString Name() { return name; }
  bool IsDirectory() { return dir; }

  virtual wxImage LoadImage() = 0;

  EntryIter FirstChild() { return children.begin(); }
  EntryIter EndChild() { return children.end(); }

  EntryIter begin() { return FirstChild(); }
  EntryIter end() { return EndChild(); }

  virtual ~Entry() {
    for (auto entry : children) {
      delete entry;
    }
  }

  void PrintChildren(int level = 0) {
    for (int i = 0; i < level; i++)
      std::cout << "--";
    std::cout << Name() << std::endl;
    for (auto child : children) {
      child->PrintChildren(level + 1);
    }
  }

private:
  std::vector<Entry *> children;
  wxString name;
  bool dir;

protected:
  void AddChild(Entry *entry) { children.push_back(entry); }
  void SetName(const wxString &_name) { name = _name; }
  void SetIsDirectory(const bool &_dir) { dir = _dir; }
};

class EntryItemData : public wxTreeItemData {
private:
  Entry *entry;

public:
  EntryItemData(Entry *e) : wxTreeItemData(), entry(e){};
  virtual ~EntryItemData() { delete entry; }

  Entry *Get() { return entry; }
};

#endif // ZIPPICVIEW_ENTRY_H
