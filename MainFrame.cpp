#include "MainFrame.h"
#include <wx/filepicker.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "ZipPicView") {
  auto statusBar = CreateStatusBar();
  SetStatusText("Welcome to ZipPicView!");

  auto outerSizer = new wxBoxSizer(wxVERTICAL);

  auto notebook = new wxNotebook(this, wxID_ANY);

  auto splitter = new wxSplitterWindow(notebook, wxID_ANY);
  splitter->SplitVertically(new wxTreeCtrl(splitter),
                            new wxButton(splitter, wxID_ANY, "Test"));

  notebook->AddPage(splitter, "Browse");
  notebook->AddPage(new wxButton(notebook, wxID_ANY, "Test"), "2");

  outerSizer->Add(new wxFilePickerCtrl(this, wxID_ANY), 0, wxALL, 10);
  outerSizer->Add(notebook, 1, wxEXPAND | wxALL, 10);

  SetSizerAndFit(outerSizer);
}

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)

wxEND_EVENT_TABLE();
// clang-format on
