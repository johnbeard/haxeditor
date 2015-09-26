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
		Director* director,
		const HaxStringRenderer& renderer) :
			HaxFrame(),
			wxWindow(parent, id, pos, size),
			m_caretPos(20, 20),
			m_bmpBuffer(0, 0),
			m_renderer(renderer),
			m_director(director)
{
	wxColour c(*wxWHITE);
	SetBackgroundColour(c);

	SetWindowStyleFlag(wxBORDER_SUNKEN);

	m_state.offset = 0;
	m_state.m_margin = wxPoint(3, 0);

	Bind(wxEVT_PAINT, &HexFrame::Paint, this);
	Bind(wxEVT_SIZE, &HexFrame::OnSize, this);
	Bind(wxEVT_KEY_DOWN, &HexFrame::OnKeyboardInput, this);
	Bind(wxEVT_LEFT_DOWN, &HexFrame::OnLeftMouseDown, this);

	m_caret = new wxCaret(this, m_director->GetCharSize());
	SetCaret(m_caret);
	m_caret->Show(false);
}

HexFrame::~HexFrame()
{
	Unbind(wxEVT_PAINT, &HexFrame::Paint, this);
	Unbind(wxEVT_SIZE, &HexFrame::OnSize, this);
	Unbind(wxEVT_KEY_DOWN, &HexFrame::OnKeyboardInput, this);
	Unbind(wxEVT_LEFT_DOWN, &HexFrame::OnLeftMouseDown, this);
}

void HexFrame::onCharSizeChanged()
{
	const wxSize charSize(m_director->GetCharSize());
	m_caret->SetSize(charSize);
	moveCaret();
	DataChanged(false);
}

void HexFrame::moveCaret()
{
	if (!m_caret)
		return;

	const wxSize charSize(m_director->GetCharSize());

	m_caret->SetSize(charSize);
	m_caret->Move(m_caretPos.x * charSize.x,
			m_caretPos.y * charSize.y);
}

void HexFrame::DataChanged(bool force)
{
	moveCaret();

	if (m_caret->IsVisible() != isCaretVisible())
		m_caret->Show(isCaretVisible());

	m_pendingState.m_charSize = m_director->GetCharSize();

	// could use a different line spacing
	m_pendingState.m_rows = m_director->GetNumRowsToShow();
	m_pendingState.m_cols = m_renderer.GetWidth();

	// see if we need to redraw (force to override and recompute anyway!)
	if (!force && !m_state.redrawNeeded(m_pendingState))
	{
		//std::cout << "skipping" << std::endl;

		// but still need to redraw, even if the bmp is the same
		Refresh();
		return;
	}
	//std::cout << "drawing " << m_pendingState.m_rows << std::endl;

	m_state = m_pendingState;

	// FIXME
	int rowH = m_state.m_charSize.y + m_state.m_margin.y;
	const int bmpH = m_state.m_rows * rowH + m_state.m_margin.y;
	const int bmpW = GetSize().GetWidth();

	// can't make zero sized bitmaps
	if (bmpH == 0 || bmpW == 0)
		return;

	m_bmpBuffer.Create(bmpW, bmpH);

	wxMemoryDC mdc;
	mdc.SetFont(m_director->GetTextAttr().GetFont());

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

void HexFrame::SetOffset(uint64_t newOffset)
{
	//TODO fix width access
	m_pendingState.offset = newOffset * m_renderer.GetWidth();
	DataChanged(false);
}

void HexFrame::OnSize(wxSizeEvent& /*event*/)
{
//	std::cout << "Height " << GetSize().GetHeight() << std::endl;
	// we resize - we might need to redraw the image
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

unsigned HexFrame::GetMinimumWidthForData() const
{
	const unsigned cells = m_renderer.GetCellsPerRow();
	const unsigned cellW = m_renderer.GetCellChars()
			* m_director->GetCharSize().GetWidth();

	const unsigned margin = 5;
	return cells * cellW + margin;
}

void HexFrame::OnKeyboardInput(wxKeyEvent& event)
{
	std::cout << "HexFrame key: " << std::endl;

	// can't handle the key here, maybe someone higher wants it?
	wxPostEvent(GetParent(), event);
}

void HexFrame::OnLeftMouseDown(wxMouseEvent& event)
{

}
