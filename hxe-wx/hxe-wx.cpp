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

	HexFrame* hf;
};

enum
{
	ID_Hello = 1
};

void MyFrame::SetData()
{
	hf = new HexFrame(this, wxID_ANY, wxDefaultPosition, wxSize(300, 300));
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);
	sz->Add(hf, 0, wxALIGN_RIGHT, 10);
	SetSizer(sz);

	sz->Show(true);

	wxColour c(*wxWHITE);
	hf->SetBackgroundColour(c);

	char data[1024];

	for (int i = 0, e = 1024; i < e; ++i)
	{
		data[i] = 2 * i;
	}

	hf->SetData(data, 1024);
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP( HaxEditorWX);

bool HaxEditorWX::OnInit()
{
	MyFrame *frame = new MyFrame("Hello World", wxPoint(50, 50),
			wxSize(450, 450));
	frame->Show(true);
	return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
		wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
			"Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("Welcome to wxWidgets!");

	Bind(wxEVT_CREATE, &MyFrame::OnCreate, this, wxID_ANY);

	// bind menu events
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);

	SetData();
}
void MyFrame::OnCreate(wxCommandEvent& /*event*/)
{
	std::cout << "created" << std::endl;
}

void MyFrame::OnExit(wxCommandEvent& /*event*/)
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& /*event*/)
{
	SetData();
	wxMessageBox("This is a wxWidgets' Hello world sample", "About Hello World",
	wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& /*event*/)
{
	wxLogMessage
	( "Hello world from wxWidgets!");
}
