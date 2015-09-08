/*
 * hexframe.cpp
 *
 *  Created on: 8 Sep 2015
 *      Author: john
 */

#include "hexframe.h"

#include "wx/wx.h"
#include "wx/graphics.h"

#include <iomanip>
#include <sstream>

HexFrame::HexFrame(wxWindow* parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size) :
			wxScrolledWindow(parent, id, pos, size, wxSUNKEN_BORDER),
			m_Margin(3, 3),
			m_Caret(0, 0),
			m_Window(10, 10),
			m_data(nullptr),
			m_len(0)
{
	Bind(wxEVT_PAINT, &HexFrame::Paint, this);
}

void HexFrame::SetData(char* data, uint64_t len)
{
	m_data = data;
	m_len = len;

	internalBmp.reset(new wxBitmap(
			this->GetSize().GetWidth(), this->GetSize().GetHeight()));

	std::unique_ptr<wxMemoryDC> mdc(new wxMemoryDC());

	mdc->SelectObject(*internalBmp);
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

		std::stringstream ss;
		ss << std::hex << std::uppercase << std::setfill('0');

		for (int x = 0; x < m_Window.x; ++x)
		{
			ss << std::setw(2) << static_cast<unsigned>(static_cast<uint8_t>(data[offset])) << " ";
			offset += 1;
		}

		mdc->DrawText(ss.str(), xPos, yPos);
	}
}

void HexFrame::Paint(wxPaintEvent& /*event*/)
{
	std::cout << "Paint" << std::endl;

	if (!internalBmp.get())
		return;

	static int sdfsdf = 0;

	sdfsdf += 10;

	sdfsdf = (sdfsdf % 50);

	wxClientDC dc( this );
	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

	gc->DrawBitmap(*internalBmp, 0.0, 0.0,
			dc.GetSize().GetWidth(), dc.GetSize().GetHeight());
}
