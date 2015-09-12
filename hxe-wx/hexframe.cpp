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

void HexFrame::DataChanged()
{
	m_bmpBuffer.Create(GetSize().GetWidth(), GetSize().GetHeight());
	wxMemoryDC mdc;

	mdc.SelectObject(m_bmpBuffer);
	mdc.Clear();
	mdc.SetFont(m_textAttr.GetFont());

	const wxSize charSize(mdc.GetCharWidth(), mdc.GetCharHeight());

	// offset into the buffer
	uint64_t offset = 0;

	// number of text lines we can fit
	const int usableH = GetSize().GetHeight() - m_Margin.y * 2;
	const int numLines = usableH / (charSize.y + m_Margin.y) + 1;

	int yPos = m_Margin.y;
	for ( int y = 0 ; y < numLines; ++y )
	{
		int xPos = m_Margin.x;

		const std::string row(m_renderer.RenderLine(offset));

		mdc.DrawText(row, xPos, yPos);

		offset += m_renderer.GetWidth();
		yPos += charSize.y + m_Margin.y;
	}

	Refresh();
}

void HexFrame::OnSize(wxSizeEvent& /*event*/)
{
//	std::cout << "Height " << GetSize().GetHeight() << std::endl;
	DataChanged();
}

void HexFrame::Paint(wxPaintEvent& /*event*/)
{
	wxClientDC dc( this );
	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

	const int w = m_bmpBuffer.GetWidth();
	const int h = m_bmpBuffer.GetHeight();

	gc->DrawBitmap(m_bmpBuffer, 0.0, 0.0, w, h);
}
