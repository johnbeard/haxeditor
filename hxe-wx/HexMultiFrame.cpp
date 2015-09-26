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
		HaxDocumentMultiFrame(doc)
{
	const int lineSize = 10;

	m_hexRenderer.reset(new HaxHexRenderer(doc));
	m_hexRenderer->SetWidth(lineSize);

	m_addrRenderer.reset(new HaxAddressRenderer(doc));
	m_addrRenderer->SetWidth(lineSize);

	m_textRenderer.reset(new HaxTextRenderer(doc));
	m_textRenderer->SetWidth(lineSize);

	m_hexPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(m_hexPanelSizer);

	std::unique_ptr<HexFrame> f;

	f = std::unique_ptr<HexFrame>(new HexFrame(
				this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
				this, *m_addrRenderer));
	m_frames.push_back(f.get());
	m_hexPanelSizer->Add(f.get(), 0, wxALIGN_RIGHT | wxEXPAND, 0);
	f.release();

	f = std::unique_ptr<HexFrame>(new HexFrame(
			this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			this, *m_hexRenderer));
	m_frames.push_back(f.get());
	m_hexPanelSizer->Add(f.release(), 100, wxALIGN_RIGHT | wxEXPAND, 0);
	f.release();

	f = std::unique_ptr<HexFrame>(new HexFrame(
			this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			this, *m_textRenderer));
	m_frames.push_back(f.get());
	m_hexPanelSizer->Add(f.release(), 0, wxALIGN_RIGHT | wxEXPAND, 0);
	f.release();

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
	const uint64_t dataRows = getTotalNumRows();

	const unsigned rows = GetNumVisibleRows();
	m_hugeScrollBar->SetScrollbar(GetRowOffset(), rows,
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

	setNumVisibleRows(visibleRows);

	// recompute the scrollbar properties
	AdjustScrollBar();

	for (auto& f: m_frames)
	{
		const unsigned frameW = f->GetMinimumWidthForData();
		f->SetMinSize(wxSize(frameW, 10));
	}
}

void HexMultiFrame::OnOffsetScroll(wxScrollEvent& /*event*/)
{
	// take the position from the HugeScrollBar
	// (which must have already processed the event!)
	SetOffset(m_hugeScrollBar->GetThumbPosition());
	//std::cout << "hmf offset scroll " << m_rowOffset << std::endl;
	updateOffset();
}

void HexMultiFrame::updateOffset()
{
	const uint64_t offset = GetRowOffset();

	for (auto& f: m_frames)
	{
		f->SetOffset(offset);
	}

	AdjustScrollBar();
}

void HexMultiFrame::OnMouseWheel(wxMouseEvent& event)
{
	if (event.GetWheelRotation() == 0)
		return;

	const bool goingDown = event.GetWheelRotation() < 0;
	const int lineDelta = event.GetLinesPerAction();

	scrollLines(lineDelta * (goingDown ? 1 : -1));
}

void HexMultiFrame::OnKeyboardInput(wxKeyEvent& event)
{
	std::cout << "key: " << event.GetKeyCode() << std::endl;

	const int keyCode = event.GetKeyCode();
	switch (keyCode)
	{
	case WXK_HOME:
		scrollToStart();
		break;
	case WXK_END:
		scrollToEnd();
		break;
	case WXK_PAGEDOWN:
		scrollPages(1);
		break;
	case WXK_PAGEUP:
		scrollPages(-1);
		break;
	default:
		// unhandled key
		event.Skip();
		break;
	}
}
