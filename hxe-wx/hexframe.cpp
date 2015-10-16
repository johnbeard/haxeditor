/*
 * hexframe.cpp
 *
 *  Created on: 8 Sep 2015
 *      Author: john
 */

#include "hexframe.h"

#include "wx/wx.h"
#include "wx/graphics.h"

class HexCaret: public wxWindow
{
public:
	HexCaret(wxWindow* parent, const wxSize& size):
		wxWindow(),
		m_blinkState(true),
		m_blinkTimerMs(750)
	{
		SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);

		// create after the BG settings
		Create(parent, wxID_ANY);

		SetSize(size);

		Bind(wxEVT_PAINT, &HexCaret::Paint, this);
		m_blinkTimer.Bind(wxEVT_TIMER, &HexCaret::OnBlinkTimer, this);

		m_blinkTimer.Start(m_blinkTimerMs, false);
	}

	~HexCaret()
	{

		Unbind(wxEVT_PAINT, &HexCaret::Paint, this);
	}

private:

	void Paint(wxPaintEvent& /*event*/)
	{
	    wxPaintDC dc(this);
	    std::unique_ptr<wxGraphicsContext> gdc(wxGraphicsContext::Create(dc));

	    gdc->SetPen(*wxBLACK_PEN);

	    if (m_blinkState)
	    {
	    	gdc->SetBrush(wxBrush(wxColour(255, 0, 0, 50)));
	    }
	    else
	    {
	    	gdc->SetBrush(*wxTRANSPARENT_BRUSH);
	    }

    	const wxSize size = GetSize();
    	gdc->DrawRectangle(0, 0, size.x - 1, size.y - 1);
	}

	void OnBlinkTimer(wxTimerEvent& /*event*/)
	{
		m_blinkState = !m_blinkState;
		Refresh();
	}

	wxTimer m_blinkTimer;
	bool m_blinkState;
	int m_blinkTimerMs;
};

HexFrame::HexFrame(wxWindow* parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size,
		Director* director,
		HaxStringRenderer& renderer) :
			HaxFrame(),
			wxWindow(parent, id, pos, size),
			m_caretPos(5, 20),
			m_bmpBuffer(0, 0),
			m_renderer(renderer),
			m_director(director),
			m_bgColour(*wxWHITE)
{
	wxColour c(*wxWHITE);
	SetBackgroundColour(c);

	SetWindowStyleFlag(wxBORDER_SUNKEN);

	m_state.offset = 0;
	m_state.m_margin = wxPoint(3, 0);

	m_caret = new HexCaret(this, m_director->GetCharSize());
	m_caret->Show();

	Bind(wxEVT_PAINT, &HexFrame::Paint, this);
	Bind(wxEVT_SIZE, &HexFrame::OnSize, this);
	Bind(wxEVT_KEY_DOWN, &HexFrame::OnKeyboardInput, this);
	m_caret->Bind(wxEVT_KEY_DOWN, &HexFrame::OnKeyboardInput, this);
	Bind(wxEVT_LEFT_DOWN, &HexFrame::OnLeftMouseDown, this);
	Bind(wxEVT_SET_FOCUS, &HexFrame::OnFocusSet, this);
	Bind(wxEVT_KILL_FOCUS, &HexFrame::OnFocusSet, this);
}

HexFrame::~HexFrame()
{
	Unbind(wxEVT_PAINT, &HexFrame::Paint, this);
	Unbind(wxEVT_SIZE, &HexFrame::OnSize, this);
	Unbind(wxEVT_KEY_DOWN, &HexFrame::OnKeyboardInput, this);
	Unbind(wxEVT_LEFT_DOWN, &HexFrame::OnLeftMouseDown, this);

	Unbind(wxEVT_SET_FOCUS, &HexFrame::OnFocusSet, this);
	Unbind(wxEVT_KILL_FOCUS, &HexFrame::OnFocusSet, this);
}

void HexFrame::onCharSizeChanged()
{
	const wxSize charSize(m_director->GetCharSize());
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
	dc.SetBrush(wxBrush(m_bgColour));
	dc.DrawRectangle(0, 0, dc.GetSize().x, dc.GetSize().y);

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

void HexFrame::moveCaretDown(int lines)
{
	// TODO: do this by just offset and then callback via the director
	m_caretPos.y += lines;
	moveCaret();
}

void HexFrame::moveCaretRight(int cols)
{
	m_caretPos.x += cols;
	moveCaret();
}

void HexFrame::OnKeyboardInput(wxKeyEvent& event)
{
	std::cout << "HexFrame key: " << std::endl;

	// handle movements in the frame because the distance depends on the
	// renderer...
	const int keyCode = event.GetKeyCode();

	switch (keyCode)
	{
	case WXK_UP:
	case WXK_DOWN:
		m_renderer.ReqMoveCaretDown((keyCode == WXK_DOWN) ? 1 : -1);
		break;
	case WXK_LEFT:
	case WXK_RIGHT:
		m_renderer.ReqMoveCaretRight((keyCode == WXK_RIGHT) ? 1 : -1);
		break;
	}

	// can't handle the key here, maybe someone higher wants it?
	wxPostEvent(GetParent(), event);
}

void HexFrame::OnLeftMouseDown(wxMouseEvent& event)
{
	// pass it on until we can deal with it? so as to avoid focus loss
	event.Skip();
}

void HexFrame::OnFocusSet(wxFocusEvent& /*event*/)
{
	// TOD: needs to be more persistant than just focus?
	if (HasFocus())
		m_bgColour = wxColour(255, 200, 255);
	else
		m_bgColour = wxColour(255,255,255);

	DataChanged(true); // FIXME force

}
