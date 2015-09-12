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
			wxScrolledWindow(parent, id, pos, size, wxSUNKEN_BORDER),
			m_Margin(3, 3),
			m_Caret(0, 0),
			m_bmpBuffer(0, 0),
			m_renderer(renderer)
{
	wxColour c(*wxWHITE);
	SetBackgroundColour(c);

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

	Bind(wxEVT_PAINT, &HexFrame::Paint, this);
	Bind(wxEVT_SIZE, &HexFrame::OnSize, this);
}

void HexFrame::DataChanged(bool force)
{
	wxMemoryDC mdc;
	mdc.SetFont(m_textAttr.GetFont());

	const wxSize charSize(mdc.GetCharWidth(), mdc.GetCharHeight());

	State newState;

	// offset into the buffer
	newState.offset = 0;

	// number of text lines we can fit (add one at the end so we always fill
	// the space)
	//only use one margin, the other is in the last row
	const int usableH = GetSize().GetHeight() - m_Margin.y;
	// could use a different line spacing
	const int rowH = charSize.y + m_Margin.y;
	newState.m_rows = (usableH / rowH) + 1;
	newState.m_cols = m_renderer.GetWidth();

	// see if we need to redraw (force to override and recompute anyway!)
	if (!force && (m_state == newState))
	{
		//std::cout << "skipping" << std::endl;

		// but still need to redraw, even if the bmp is the same
		Refresh();
		return;
	}
	//std::cout << "drawing" << std::endl;

	m_state = std::move(newState);

	const int bmpH = m_state.m_rows * rowH + m_Margin.y;
	m_bmpBuffer.Create(GetSize().GetWidth(), bmpH);

	mdc.SelectObject(m_bmpBuffer);
	mdc.Clear();

	int yPos = m_Margin.y;
	uint64_t lineOffset = m_state.offset;
	for ( int y = 0 ; y < m_state.m_rows; ++y )
	{
		int xPos = m_Margin.x;

		const std::string row(m_renderer.RenderLine(lineOffset));

		mdc.DrawText(row, xPos, yPos);

		lineOffset += m_state.m_cols;
		yPos += charSize.y + m_Margin.y;
	}

	Refresh();
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
