/*
 * hexframe.cpp
 *
 *  Created on: 8 Sep 2015
 *      Author: john
 */

#include "hexframe.h"

#include "wx/wx.h"
#include "wx/graphics.h"

HexFrame::HexFrame(wxWindow* parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size,
		const HaxStringRenderer& renderer) :
			wxWindow(parent, id, pos, size),
			m_Caret(0, 0),
			m_bmpBuffer(0, 0),
			m_renderer(renderer)
{
	wxColour c(*wxWHITE);
	SetBackgroundColour(c);

	SetWindowStyleFlag(wxBORDER_SUNKEN);

	m_textAttr = wxTextAttr(
			wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ),
			wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ),
			wxFont(
					10,
					wxFONTFAMILY_MODERN,	// family
					wxFONTSTYLE_NORMAL,	// style
					wxFONTWEIGHT_NORMAL,// weight
					false,				// underline
					wxT(""),			// facename
					wxFONTENCODING_UTF8
			)
	);

	m_state.m_margin = wxPoint(3, 0);

	Bind(wxEVT_PAINT, &HexFrame::Paint, this);
	Bind(wxEVT_SIZE, &HexFrame::OnSize, this);
}

void HexFrame::DataChanged(bool force)
{
	wxMemoryDC mdc;
	mdc.SetFont(m_textAttr.GetFont());

	State newState;

	// offset into the buffer
	newState.offset = 0;

	newState.m_charSize = wxSize(mdc.GetCharWidth(), mdc.GetCharHeight());
	// number of text lines we can fit (add one at the end so we always fill
	// the space)
	//only use one margin, the other is in the last row
	const int usableH = GetSize().GetHeight() - m_state.m_margin.y;
	// could use a different line spacing
	const int rowH = newState.m_charSize.y + m_state.m_margin.y;
	newState.m_rows = (usableH / rowH) + 1;
	newState.m_cols = m_renderer.GetWidth();

	// see if we need to redraw (force to override and recompute anyway!)
	if (!force && (m_state.redrawNeeded(newState)))
	{
		//std::cout << "skipping" << std::endl;

		// but still need to redraw, even if the bmp is the same
		Refresh();
		return;
	}
	//std::cout << "drawing" << std::endl;

	m_state = std::move(newState);

	const int bmpH = m_state.m_rows * rowH + m_state.m_margin.y;
	const int bmpW = GetSize().GetWidth();

	// can't make zero sized bitmaps
	if (bmpH == 0 || bmpW == 0)
		return;

	m_bmpBuffer.Create(bmpW, bmpH);

	mdc.SelectObject(m_bmpBuffer);
	mdc.Clear();

	drawToBitmap(mdc);

	Refresh();
}

void HexFrame::drawToBitmap(wxDC& dc)
{
	int yPos = m_state.m_margin.y;
	uint64_t lineOffset = m_state.offset;
	for ( int y = 0 ; y < m_state.m_rows; ++y )
	{
		int xPos = m_state.m_margin.x;

		const std::string row(m_renderer.RenderLine(lineOffset));

		dc.DrawText(row, xPos, yPos);

		lineOffset += m_state.m_cols;
		yPos += m_state.m_charSize.y + m_state.m_margin.y;
	}
}

void HexFrame::OnSize(wxSizeEvent& /*event*/)
{
//	std::cout << "Height " << GetSize().GetHeight() << std::endl;
	DataChanged(false);
}

void HexFrame::Paint(wxPaintEvent& /*event*/)
{
	wxClientDC dc( this );
	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

	const int w = m_bmpBuffer.GetWidth();
	const int h = m_bmpBuffer.GetHeight();

	if (w == 0 || h == 0)
		return;

	gc->DrawBitmap(m_bmpBuffer, 0.0, 0.0, w, h);
}
