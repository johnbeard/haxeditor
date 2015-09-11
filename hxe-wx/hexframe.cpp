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
			m_Window(10, 10),
			m_bmpBuffer(0, 0),
			m_renderer(renderer)
{
	Bind(wxEVT_PAINT, &HexFrame::Paint, this);
}

void HexFrame::DataChanged()
{
	m_bmpBuffer.SetWidth(GetSize().GetWidth());
	m_bmpBuffer.SetHeight(GetSize().GetHeight());

	std::unique_ptr<wxMemoryDC> mdc(new wxMemoryDC());

	mdc->SelectObject(m_bmpBuffer);
	mdc->Clear();

	wxTextAttr attrs(
			wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ),
			wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ),
			wxFont(
					10,
					wxFONTFAMILY_MODERN,	// family
					wxFONTSTYLE_NORMAL,	// style
					wxFONTWEIGHT_NORMAL,// weight
					false,				// underline
					wxT(""),			// facename
					wxFONTENCODING_UTF8)
	);

	mdc->SetFont( attrs.GetFont() );

	const wxSize charSize(mdc->GetCharWidth(), mdc->GetCharHeight());

	// offset into the buffer
	uint64_t offset = 0;

	int yPos = m_Margin.y;
	for ( int y = 0 ; y < m_Window.y; ++y )
	{
		yPos += charSize.y + m_Margin.y;
		int xPos = m_Margin.x;

		const std::string row(m_renderer.RenderLine(offset));

		mdc->DrawText(row, xPos, yPos);

		offset += m_renderer.GetWidth();
	}
}

void HexFrame::Paint(wxPaintEvent& /*event*/)
{
	if (!m_bmpBuffer.GetWidth())
		return;

	wxClientDC dc( this );
	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

	gc->DrawBitmap(m_bmpBuffer, 0.0, 0.0,
			dc.GetSize().GetWidth(), dc.GetSize().GetHeight());
}
