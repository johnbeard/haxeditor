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

	HexFrame* hexFrame;
	HexFrame* addrFrame;

	std::unique_ptr<HaxDocument> m_doc;

	std::unique_ptr<HaxStringRenderer> m_hexRenderer;
	std::unique_ptr<HaxAddressRenderer> m_addrRenderer;
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
	wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);

	m_doc.reset(new HaxDocument());

	const int lineSize = 10;

	m_hexRenderer.reset(new HaxHexRenderer(*m_doc));
	m_hexRenderer->SetWidth(lineSize);

	m_addrRenderer.reset(new HaxAddressRenderer(*m_doc));
	m_addrRenderer->SetWidth(lineSize);

	const int height = 300;

	hexFrame = new HexFrame(this, wxID_ANY, wxDefaultPosition, wxSize(300, height),
			*m_hexRenderer);

	addrFrame = new HexFrame(this, wxID_ANY, wxDefaultPosition, wxSize(100, height),
			*m_addrRenderer);

	sz->Add(addrFrame, 0, wxALIGN_RIGHT | wxEXPAND, 10);
	sz->Add(hexFrame, 100, wxALIGN_RIGHT | wxEXPAND, 10);
	SetSizer(sz);

	sz->Show(true);
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
