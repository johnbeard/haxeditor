// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "HexMultiFrame.h"
#include "haxeditor/HaxOffsetRenderer.h"

class HaxEditorWX: public wxApp
{
public:
	virtual bool OnInit();
};

class HwxStatusBar
{
public:

	HwxStatusBar(wxStatusBar& sb):
		m_statusBar(sb)
	{
		m_statusBar.SetFieldsCount(NUM_Fields, nullptr);

		// set up the fields to start
		SetOffset(0);
	}

	void SetOffset(offset_t offset)
	{
		m_offsetRender.SetOffset(offset / BYTE);
		m_statusBar.SetStatusText(m_offsetRender.RenderOffset(), OffsetField);
	}

private:
	enum Field
	{
		OffsetField = 0,
		NUM_Fields
	};

	// the status bar - note that we are not going to own this,
	// the main frame owns it and we just hold a reference
	wxStatusBar& m_statusBar;

	// TODO: switchable renderer?
	HaxOffsetRendererDecimal m_offsetRender;
};

class MyFrame: public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MyFrame();

	void SetData();
private:

	void OnCreate(wxCommandEvent& event);

	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnExitWindow(wxCloseEvent& event);
	void OnAbout(wxCommandEvent& event);

	std::unique_ptr<HaxDocument> m_doc;
	HexMultiFrame* m_mframe;

	std::unique_ptr<HwxStatusBar> m_statusBar;
};

enum
{
	ID_Hello = 1
};

wxIMPLEMENT_APP( HaxEditorWX);

bool HaxEditorWX::OnInit()
{
	MyFrame *frame = new MyFrame("HaxEditor", wxPoint(50, 50),
			wxSize(750, 450));
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
	menuBar->Append(menuFile, wxT("&File"));
	menuBar->Append(menuHelp, wxT("&Help"));
	SetMenuBar(menuBar);

	m_statusBar = std::make_unique<HwxStatusBar>(*CreateStatusBar());

	// bind menu events
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);

	Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnExitWindow, this);

	SetData();
}

MyFrame::~MyFrame()
{
	// destroy children first because they rely on data
	// that will be deleted here, but events can prompt accesses
	// destruction and the children's own dtors
	DestroyChildren();

	Unbind(wxEVT_CLOSE_WINDOW, &MyFrame::OnExitWindow, this);
}

void MyFrame::SetData()
{
	m_doc = std::make_unique<HaxDocument>();

	m_mframe = new HexMultiFrame(this, wxID_ANY, *m_doc);

	wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(sz);
	sz->Show(true);
	sz->Add(m_mframe, 100, wxALIGN_RIGHT | wxEXPAND, 10);

	// bind document events
	m_doc->signal_OffsetChanged.connect(sigc::mem_fun(m_statusBar.get(),
			&HwxStatusBar::SetOffset));
}

void MyFrame::OnExit(wxCommandEvent& /*event*/)
{
	Destroy();
}

void MyFrame::OnExitWindow(wxCloseEvent& event)
{
	wxCommandEvent evt;
	OnExit( evt );
	event.Skip(false);
}

void MyFrame::OnAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox(wxT("HaxEditor is a proper hex editor."), wxT("About HaxEditor"),
			wxOK | wxICON_INFORMATION);
}
