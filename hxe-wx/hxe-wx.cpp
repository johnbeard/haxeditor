// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "hexframe.h"

class HaxEditorWX: public wxApp
{
public:
	virtual bool OnInit();
};

class MyFrame: public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void SetData();
private:

	void OnCreate(wxCommandEvent& event);

	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);wxDECLARE_EVENT_TABLE();

	std::unique_ptr<HaxDocument> m_doc;
	HexMultiFrame* m_mframe;
};

enum
{
	ID_Hello = 1
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP( HaxEditorWX);

bool HaxEditorWX::OnInit()
{
	MyFrame *frame = new MyFrame("HaxEditor", wxPoint(50, 50),
			wxSize(450, 450));
	frame->Show(true);
	return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
		wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("Welcome to HaxEditor");

	// bind menu events
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);

	SetData();
}

void MyFrame::SetData()
{

	m_doc.reset(new HaxDocument());

	m_mframe = new HexMultiFrame(this, wxID_ANY, *m_doc);

	wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(sz);
	sz->Show(true);
	sz->Add(m_mframe, 100, wxALIGN_RIGHT | wxEXPAND, 10);

}

void MyFrame::OnExit(wxCommandEvent& /*event*/)
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox("HaxEditor is a proper hex editor.", "About HaxEditor",
			wxOK | wxICON_INFORMATION);
}
