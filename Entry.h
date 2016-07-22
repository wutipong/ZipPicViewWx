//
// Created by noom on 10/29/15.
//

#ifndef ZIPPICVIEW_ENTRY_H
#define ZIPPICVIEW_ENTRY_H

#include <algorithm>
#include <iostream>
#include <vector>

#include <wx/image.h>
#include <wx/stream.h>
#include <wx/string.h>
#include <wx/treebase.h>

class Entry;
typedef std::vector<Entry *>::const_iterator EntryIter;

class Entry {
public:
  wxString Name() { return name; }
  bool IsDirectory() { return dir; }

  virtual wxImage LoadImage() = 0;
  virtual void WriteStream(wxOutputStream &) = 0;

  EntryIter FirstChild() { return children.begin(); }
  EntryIter EndChild() { return children.end(); }

  EntryIter begin() { return FirstChild(); }
  EntryIter end() { return EndChild(); }

  virtual ~Entry() {
    for (auto entry : children) {
      delete entry;
    }
    children.clear();
  }

  void PrintChildren(int level = 0) {
    for (int i = 0; i < level; i++)
      std::cout << "|-";
    std::cout << Name() << std::endl;
    for (auto child : children) {
      child->PrintChildren(level + 1);
    }
  }
  size_t Count() { return children.size(); }
  Entry *Child(const size_t &i) { return children[i]; };
  Entry *operator[](const size_t &i) { return Child(i); };

  virtual bool IsRoot() const = 0;

  Entry *Parent() { return parent; }

private:
  std::vector<Entry *> children;
  wxString name;
  bool dir;
  Entry *parent = nullptr;

protected:
  void SetParent(Entry *parent) { this->parent = parent; }
  void AddChild(Entry *entry) {
    children.push_back(entry);
    entry->SetParent(this);
  }
  void SetName(const wxString &_name) { name = _name; }
  void SetIsDirectory(const bool &_dir) { dir = _dir; }

  void SortChildren() {
    std::sort(children.begin(), children.end(), [](Entry *e1, Entry *e2) {
      if (e1->IsDirectory() == e2->IsDirectory()) {
        return e1->Name() < e2->Name();
      } else if (e1->IsDirectory()) {
        return true;
      } else {
        return false;
      }
    });

    std::for_each(children.begin(), children.end(),
                  [](Entry *e) { e->SortChildren(); });
  }
};

class EntryItemData : public wxTreeItemData {
private:
  Entry *entry;

public:
  EntryItemData(Entry *e) : wxTreeItemData(), entry(e){};

  Entry *Get() { return entry; }
};

#endif // ZIPPICVIEW_ENTRY_H
