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
		m_rowOffset(0),
		m_doc(doc)
{
	const int lineSize = 10;

	m_hexRenderer.reset(new HaxHexRenderer(m_doc));
	m_hexRenderer->SetWidth(lineSize);

	m_addrRenderer.reset(new HaxAddressRenderer(m_doc));
	m_addrRenderer->SetWidth(lineSize);

	m_textRenderer.reset(new HaxTextRenderer(m_doc));
	m_textRenderer->SetWidth(lineSize);

	m_hexFrame = new HexFrame(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			this, *m_hexRenderer);

	m_addrFrame = new HexFrame(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			this, *m_addrRenderer);

	m_textFrame = new HexFrame(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			this, *m_textRenderer);

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

	// do this AFTER the real scrollbar binding (so it will have priotity),
			// because we need the HugeScrollBar to get its position before we deal
	// with the event though the wrapper in this call!
	m_hugeScrollBar = new wxHugeScrollBar( m_realScrollBar );

	m_hexPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(m_hexPanelSizer);

	m_hexPanelSizer->Add(m_addrFrame, 0, wxALIGN_RIGHT | wxALL | wxEXPAND, 0);
	m_hexPanelSizer->Add(m_hexFrame, 100, wxALIGN_RIGHT | wxEXPAND, 0);
	m_hexPanelSizer->Add(m_textFrame, 0, wxALIGN_RIGHT | wxEXPAND, 0);
	m_hexPanelSizer->Add(m_realScrollBar, 0, wxALIGN_RIGHT | wxEXPAND, 0);

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

uint64_t HexMultiFrame::getTotalNumRows() const
{
	const uint64_t dataSize = m_doc.GetDataLength();
	const unsigned lineSize = 10;

	uint64_t dataRows = dataSize / lineSize;
	const unsigned lastRowCount = dataSize % lineSize;

	if (lastRowCount)
		dataRows += 1;

	// one more row because we draw the last row off the bottom edge
	dataRows += 1;

	return dataRows;
}

uint64_t HexMultiFrame::getMaximumOffsetRow() const
{
	const uint64_t pageRows = GetNumRowsToShow();
	const uint64_t totalRows = getTotalNumRows();

	if (pageRows > totalRows)
		return 0;

	return totalRows - pageRows;
}

void HexMultiFrame::AdjustScrollBar()
{
	const uint64_t dataRows = getTotalNumRows();

	m_hugeScrollBar->SetScrollbar(m_rowOffset, m_rows, dataRows, m_rows - 1, true);
}

void HexMultiFrame::OnResize(wxSizeEvent& /*event*/)
{
	const int paneH = GetSize().GetHeight();

	const int usableH = paneH;
	// could use a different line spacing
	const int rowH = m_charSize.GetHeight();

	m_rows = usableH / rowH;

	if (usableH % rowH)
		m_rows += 1;

	// recompute the scrollbar properties
	AdjustScrollBar();

	const unsigned addrW = m_addrFrame->GetMinimumWidthForData();
	const unsigned hexW = m_hexFrame->GetMinimumWidthForData();
	const unsigned textW = m_textFrame->GetMinimumWidthForData();

	m_addrFrame->SetMinSize(wxSize(addrW, 10));
	m_hexFrame->SetMinSize(wxSize(hexW, 10));
	m_textFrame->SetMinSize(wxSize(textW, 10));
}

void HexMultiFrame::OnOffsetScroll(wxScrollEvent& /*event*/)
{
	// take the position from the HugeScrollBar
	// (which must have already processed the event!)
	m_rowOffset = m_hugeScrollBar->GetThumbPosition() ;
	//std::cout << "hmf offset scroll " << m_rowOffset << std::endl;
	updateOffset();
}

void HexMultiFrame::updateOffset()
{
	m_addrFrame->SetOffset(m_rowOffset);
	m_hexFrame->SetOffset(m_rowOffset);
	m_textFrame->SetOffset(m_rowOffset);

	AdjustScrollBar();
}

void HexMultiFrame::OnMouseWheel(wxMouseEvent& event)
{
	if (event.GetWheelRotation() == 0)
		return;

	const bool goingDown = event.GetWheelRotation() < 0;
	const uint64_t lineDelta = static_cast<uint64_t>(event.GetLinesPerAction());

	if (goingDown)
	{
		const uint64_t maxRow = getMaximumOffsetRow();

		// don't exceed the end of the document
		m_rowOffset = std::min(m_rowOffset + lineDelta, maxRow);
	}
	else
	{
		if (lineDelta > m_rowOffset)
			m_rowOffset = 0;
		else
			m_rowOffset -= lineDelta;
	}

	// notify offset changed
	updateOffset();
}

void HexMultiFrame::OnKeyboardInput(wxKeyEvent& event)
{
	std::cout << "key: " << event.GetKeyCode() << std::endl;

	switch (event.GetKeyCode())
	{
	case WXK_HOME:
		scrollToStart();
		break;
	case WXK_END:
		scrollToEnd();
		break;
	default:
		// unhandled key
		event.Skip();
		break;
	}
}

void HexMultiFrame::scrollToStart()
{
	m_rowOffset = 0;
	updateOffset();
}

void HexMultiFrame::scrollToEnd()
{
	m_rowOffset = getMaximumOffsetRow();
	updateOffset();
}
