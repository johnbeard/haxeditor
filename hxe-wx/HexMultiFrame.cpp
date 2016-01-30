/*
 * HexMultiFrame.cpp
 *
 *  Created on: 15 Sep 2015
 *      Author: John Beard
 */

#include "HexMultiFrame.h"

#include <algorithm>

HexMultiFrame::HexMultiFrame(wxWindow* parent, wxWindowID id,
		HaxDocument& doc) :
		wxScrolledWindow(parent, id),
		HaxDocumentMultiFrame(doc),
		m_doc(doc)
{
	m_hexPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(m_hexPanelSizer);

	std::unique_ptr<HexTextFrame> f;
	std::unique_ptr<StringCellRenderer> renderer;

	{
		renderer = std::make_unique<HaxAddressRenderer>(doc);
		StringCellRenderer& thisRender = *renderer;
		HaxTextCellFrame& theframe = AddNewFrame(renderer);

		f = std::unique_ptr<HexTextFrame>(new HexTextFrame(
				this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				this, thisRender, theframe));
		m_frameWindows.push_back(f.get());

		m_hexPanelSizer->Add(f.release(), 0, wxALIGN_RIGHT | wxEXPAND, 0);
	}

	{
		renderer = std::make_unique<HaxHexRenderer>(doc);
		StringCellRenderer& thisRender = *renderer;
		HaxTextCellFrame& theframe = AddNewFrame(renderer);

		f = std::unique_ptr<HexTextFrame>(new HexTextFrame(
				this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				this, thisRender, theframe));
		m_frameWindows.push_back(f.get());
		m_hexPanelSizer->Add(f.release(), 100, wxALIGN_RIGHT | wxEXPAND, 0);
		f.release();
	}

	{
		renderer = std::make_unique<HaxTextRenderer>(doc);
		StringCellRenderer& thisRender = *renderer;
		HaxTextCellFrame& theframe = AddNewFrame(renderer);

		f = std::unique_ptr<HexTextFrame>(new HexTextFrame(
				this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				this, thisRender, theframe));
		m_frameWindows.push_back(f.get());
		m_hexPanelSizer->Add(f.release(), 0, wxALIGN_RIGHT | wxEXPAND, 0);
		f.release();
	}

	m_realScrollBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition,
			wxDefaultSize, wxSB_VERTICAL);
	m_realScrollBar->Enable(true);

	m_realScrollBar->Bind( wxEVT_SCROLL_TOP, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Bind( wxEVT_SCROLL_BOTTOM, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Bind( wxEVT_SCROLL_LINEUP, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Bind( wxEVT_SCROLL_LINEDOWN, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Bind( wxEVT_SCROLL_PAGEUP, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Bind( wxEVT_SCROLL_PAGEDOWN, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Bind( wxEVT_SCROLL_THUMBTRACK, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Bind( wxEVT_SCROLL_THUMBRELEASE, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Bind( wxEVT_SCROLL_CHANGED, &HexMultiFrame::OnOffsetScroll, this );

	m_hexPanelSizer->Add(m_realScrollBar, 0, wxALIGN_RIGHT | wxEXPAND, 0);

	// do this AFTER the real scrollbar binding (so it will have priotity),
			// because we need the HugeScrollBar to get its position before we deal
	// with the event though the wrapper in this call!
	m_hugeScrollBar = new wxHugeScrollBar( m_realScrollBar );

	m_hexPanelSizer->Show(true);

	// set text size
	m_textAttr = wxTextAttr(
			wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ),
			wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ),
			wxFont(
					10,
					wxFONTFAMILY_MODERN,// family
					wxFONTSTYLE_NORMAL, // style
					wxFONTWEIGHT_NORMAL,// weight
					false,              // underline
					wxT(""),            // facename
					wxFONTENCODING_UTF8
			)
	);

	OnFontChange();

	Bind(wxEVT_SIZE, &HexMultiFrame::OnResize, this);
	Bind(wxEVT_MOUSEWHEEL, &HexMultiFrame::OnMouseWheel, this);
	Bind(wxEVT_KEY_DOWN, &HexMultiFrame::OnKeyboardInput, this);
}

HexMultiFrame::~HexMultiFrame()
{
	m_realScrollBar->Unbind( wxEVT_SCROLL_TOP, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Unbind( wxEVT_SCROLL_BOTTOM, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Unbind( wxEVT_SCROLL_LINEUP, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Unbind( wxEVT_SCROLL_LINEDOWN, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Unbind( wxEVT_SCROLL_PAGEUP, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Unbind( wxEVT_SCROLL_PAGEDOWN, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Unbind( wxEVT_SCROLL_THUMBTRACK, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Unbind( wxEVT_SCROLL_THUMBRELEASE, &HexMultiFrame::OnOffsetScroll, this );
	m_realScrollBar->Unbind( wxEVT_SCROLL_CHANGED, &HexMultiFrame::OnOffsetScroll, this );

	Unbind(wxEVT_SIZE, &HexMultiFrame::OnResize, this);
	Unbind(wxEVT_MOUSEWHEEL, &HexMultiFrame::OnMouseWheel, this);
	Unbind(wxEVT_KEY_DOWN, &HexMultiFrame::OnKeyboardInput, this);
}

void HexMultiFrame::OnFontChange()
{
	wxMemoryDC mdc;
	mdc.SetFont(m_textAttr.GetFont());
	m_charSize = wxSize(mdc.GetCharWidth(), mdc.GetCharHeight());
}

void HexMultiFrame::AdjustScrollBar()
{
	const uint64_t dataRows = m_doc.GetDataLength() / m_docView.GetRowLength();
	const uint64_t rowOffset = m_docView.GetPageOffset() / m_docView.GetRowLength();

	const unsigned rows = m_docView.GetNumVisibleRows();

	m_hugeScrollBar->SetScrollbar(rowOffset, rows,
			dataRows, rows - 1, true);
}

void HexMultiFrame::OnResize(wxSizeEvent& /*event*/)
{
	const int paneH = GetSize().GetHeight();

	const int usableH = paneH;
	// could use a different line spacing
	const int rowH = m_charSize.GetHeight();

	unsigned visibleRows = usableH / rowH;

	if (usableH % rowH)
		visibleRows += 1;

	m_docView.SetNumVisibleRows(visibleRows);

	// recompute the scrollbar properties
	// TODO should react to a signal caused by the above?
	AdjustScrollBar();

	for (auto& f: m_frameWindows)
	{
		const unsigned frameW = f->GetMinimumWidthForData();
		static_cast<HexTextFrame*>(f)->SetMinSize(wxSize(frameW, 10));
	}
}

void HexMultiFrame::OnOffsetScroll(wxScrollEvent& /*event*/)
{
	// take the position from the HugeScrollBar
	// (which must have already processed the event!)
	const uint64_t currRow = m_docView.GetPageOffset() / m_docView.GetRowLength();
	const int64_t lineDelta = m_hugeScrollBar->GetThumbPosition() - currRow;

	if (lineDelta)
		m_docView.scrollLines(lineDelta, false, false);

	//std::cout << "hmf offset scroll " << m_rowOffset << std::endl;
}

void HexMultiFrame::onPageStartChangedInt()
{
	AdjustScrollBar();
}

void HexMultiFrame::OnMouseWheel(wxMouseEvent& event)
{
	if (event.GetWheelRotation() == 0)
		return;

	const bool goingDown = event.GetWheelRotation() < 0;
	const int lineDelta = event.GetLinesPerAction();

	// scroll the page not the offset
	m_docView.scrollPageStart(lineDelta * (goingDown ? 1 : -1));
}

void HexMultiFrame::OnKeyboardInput(wxKeyEvent& event)
{
	std::cout << "key: " << event.GetKeyCode() << std::endl;

	const int keyCode = event.GetKeyCode();
	switch (keyCode)
	{
	case WXK_HOME:
		m_docView.scrollToStart();
		break;
	case WXK_END:
		m_docView.scrollToEnd();
		break;
	case WXK_PAGEDOWN:
	case WXK_PAGEUP:
		m_docView.scrollPages((keyCode == WXK_PAGEDOWN) ? 1 :-1, false);
		break;
	case WXK_UP:
	case WXK_DOWN:
		m_docView.scrollLines((keyCode == WXK_DOWN) ? 1: -1, event.ShiftDown(), true);
		break;
	case WXK_LEFT:
	case WXK_RIGHT:
		m_docView.scrollRight((keyCode == WXK_RIGHT) ? 1 : -1, event.ShiftDown());
		break;
	default:
		// unhandled key
		event.Skip();
		break;
	}
}
