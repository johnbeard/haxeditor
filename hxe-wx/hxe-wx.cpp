// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/aui/aui.h>
#endif

#include "HexMultiFrame.h"
#include "haxeditor/HaxOffsetRenderer.h"

// panels
#include "hxe-wx/panels/HwxDataInspectorPanel.h"

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

		// reuse the offset renderer for the range renderer (we probably
		// want them to be the same, but easy to change later)
		m_rangeRender.SetRenderer(&m_offsetRender);

		// set up the fields to start
		SetOffset(0);

		SetSelectionRange(HaxDocument::Selection());
	}

	void SetOffset(offset_t offset)
	{
		m_offsetRender.SetOffset(offset / BYTE);
		m_statusBar.SetStatusText(m_offsetRender.RenderOffset(), OffsetField);
	}

	void SetSelectionRange(const HaxDocument::Selection& selection)
	{
		m_rangeRender.SetRange(selection.GetStart() / BYTE, selection.GetEnd() / BYTE);

		m_statusBar.SetStatusText(m_rangeRender.RenderRange(), RangeField);
		m_statusBar.SetStatusText(m_rangeRender.RenderRangeSize(), RangeSizeField);
	}

private:
	enum Field
	{
		OffsetField = 0,
		RangeField,
		RangeSizeField,
		NUM_Fields
	};

	// the status bar - note that we are not going to own this,
	// the main frame owns it and we just hold a reference
	wxStatusBar& m_statusBar;

	// TODO: switchable renderer?
	HaxOffsetRendererDecimal m_offsetRender;

	HaxRangeRenderer m_rangeRender;
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

	wxAuiManager* m_mainAui;

	// the main document tabbed notebook
	wxAuiNotebook* m_notebook;

	std::unique_ptr<HaxDocument> m_doc;
	HexMultiFrame* m_mframe;

	std::unique_ptr<HwxStatusBar> m_statusBar;

	std::unique_ptr<DataInspectorPanel> m_diPanel; //TODO move this into the main lib

	HwxDataInspectorPanel* m_dataInspPanel;
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

	// set up the AUI
	m_mainAui = new wxAuiManager(this);

	m_statusBar = std::make_unique<HwxStatusBar>(*CreateStatusBar());

	// main document notebook
	m_notebook = new wxAuiNotebook( this, wxID_ANY,
			wxDefaultPosition, wxDefaultSize,
			wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_SPLIT | wxAUI_NB_WINDOWLIST_BUTTON);
	m_notebook->SetArtProvider(new wxAuiDefaultTabArt);

	m_mainAui->AddPane(m_notebook, wxAuiPaneInfo().
			CaptionVisible(false).
			Name("Document Browser").
			Caption("Document Browser").
			MinSize(wxSize(150,100)).
			CloseButton(false).
			Center().Layer(1));

	//TODO move into core
	m_diPanel = std::make_unique<DataInspectorPanel>();

	m_dataInspPanel = new HwxDataInspectorPanel(*m_diPanel, this);

	m_mainAui->AddPane(m_dataInspPanel,
			wxAuiPaneInfo().
				Name("Data Inspector").
				Caption("Data Inspector").
				TopDockable(false).
				BottomDockable(false).
				BestSize(wxSize(174,218)).
				Resizable(false).
				Show(true).
				Left().Layer(1).Position(0)
	);

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

	m_mainAui->UnInit();
}

void MyFrame::SetData()
{
	m_doc = std::make_unique<HaxDocument>();

	m_mframe = new HexMultiFrame(this, wxID_ANY, *m_doc);

	m_notebook->AddPage(m_mframe, "File Name", true );
	m_mframe->Show();

	m_diPanel->SetPanelDocument(*m_doc);

	m_mainAui->Update(); // TODO needed?

	// bind document events
	m_doc->signal_OffsetChanged.connect(sigc::mem_fun(m_statusBar.get(),
			&HwxStatusBar::SetOffset));

	m_doc->signal_SelectionChanged.connect(sigc::mem_fun(m_statusBar.get(),
			&HwxStatusBar::SetSelectionRange));
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
