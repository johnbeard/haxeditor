/*
 * hexframe.cpp
 *
 *  Created on: 8 Sep 2015
 *      Author: John Beard
 */

#include <hxe-wx/HexTextFrame.h>
#include "wx/wx.h"
#include "wx/graphics.h"

#include <cstdlib>

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

HexTextFrame::HexTextFrame(wxWindow* parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size,
		Director* director,
		HaxStringRenderer& renderer,
		HaxDocument::Selection& selection) :
			HaxFrame(selection),
			wxWindow(parent, id, pos, size),
			m_caretPos(0, 0),
			m_bmpBuffer(0, 0),
			m_renderer(renderer),
			m_director(director),
			m_bgColour(*wxWHITE),
			m_selectionRenderer(renderer)
{
	wxColour c(*wxWHITE);
	SetBackgroundColour(c);

	SetWindowStyleFlag(wxBORDER_SUNKEN);

	m_state.offset = 0;
	m_state.m_margin = wxPoint(3, 0);

	m_caret = new HexCaret(this, m_director->GetCharSize());
	m_caret->Show();

	Bind(wxEVT_PAINT, &HexTextFrame::Paint, this);
	Bind(wxEVT_SIZE, &HexTextFrame::OnSize, this);
	Bind(wxEVT_KEY_DOWN, &HexTextFrame::OnKeyboardInput, this);
	m_caret->Bind(wxEVT_KEY_DOWN, &HexTextFrame::OnKeyboardInput, this);
	Bind(wxEVT_LEFT_DOWN, &HexTextFrame::OnLeftMouseDown, this);
	Bind(wxEVT_SET_FOCUS, &HexTextFrame::OnFocusSet, this);
	Bind(wxEVT_KILL_FOCUS, &HexTextFrame::OnFocusSet, this);
}

HexTextFrame::~HexTextFrame()
{
	Unbind(wxEVT_PAINT, &HexTextFrame::Paint, this);
	Unbind(wxEVT_SIZE, &HexTextFrame::OnSize, this);
	Unbind(wxEVT_KEY_DOWN, &HexTextFrame::OnKeyboardInput, this);
	Unbind(wxEVT_LEFT_DOWN, &HexTextFrame::OnLeftMouseDown, this);

	Unbind(wxEVT_SET_FOCUS, &HexTextFrame::OnFocusSet, this);
	Unbind(wxEVT_KILL_FOCUS, &HexTextFrame::OnFocusSet, this);
}

void HexTextFrame::onCharSizeChanged()
{
	const wxSize charSize(m_director->GetCharSize());
	DataChanged(false);
}

void HexTextFrame::moveCaret()
{
	if (!m_caret)
		return;

	const wxSize charSize(m_director->GetCharSize());

	m_caret->SetSize(charSize);
	m_caret->Move(m_caretPos.x * charSize.x,
			m_caretPos.y * charSize.y);
}

void HexTextFrame::DataChanged(bool force)
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

	// set up selection renderer //FIXME where does this go?
	{
		SelectionRenderer::Layout selLayout;

		selLayout.charH = m_state.m_charSize.y;
		selLayout.charW = m_state.m_charSize.x;

		// a full char intercell gap
		selLayout.interCellGap = m_state.m_charSize.x;

		selLayout.interRowGap = m_state.m_margin.y; // reuse of margin.y for gap?

		m_selectionRenderer.SetLayout(selLayout);
	}

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

void HexTextFrame::drawToBitmap(wxDC& dc)
{
	dc.SetBrush(wxBrush(m_bgColour));
	dc.DrawRectangle(0, 0, dc.GetSize().x, dc.GetSize().y);

	// draw the selection outline
	//TODO move out to separate window?
	dc.SetBrush(wxColour(0, 100, 255, 50));
	dc.SetPen(wxColour(50, 50, 50, 255));
	dc.DrawPolygon(&m_selectionPolygon);

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

void HexTextFrame::SetOffset(uint64_t newOffset)
{
	m_pendingState.offset = newOffset;
	DataChanged(false);
}

void HexTextFrame::SetCaretPosition(uint64_t newOffset)
{
	const offset_t caretPosInPage = newOffset - m_state.offset;

	const unsigned chPerCell = m_renderer.GetCellChars();
	const unsigned bitsPerChar = m_renderer.GetBitsPerChar();
	const unsigned bitsPerCell = bitsPerChar * chPerCell;

	// work out the new care position
	const unsigned w = m_renderer.GetWidth();
	uint64_t div = caretPosInPage / w; // in rows
	uint64_t rem = caretPosInPage % w; // in bits

	uint64_t cells = rem / bitsPerCell;
	uint64_t chars = (rem % bitsPerCell) / bitsPerChar;

	m_caretPos.y = div;
	m_caretPos.x = cells * chPerCell + chars;

	// add the intercell gaps
	if (cells)
		m_caretPos.x += cells;

	std::cout << "new caret pos for offset " << newOffset << ": "
			<< m_caretPos.x << ", " << m_caretPos.y << std::endl;

	moveCaret();
}

void HexTextFrame::OnSize(wxSizeEvent& /*event*/)
{
//	std::cout << "Height " << GetSize().GetHeight() << std::endl;
	// we resize - we might need to redraw the image
	DataChanged(false);
}

void HexTextFrame::Paint(wxPaintEvent& /*event*/)
{
	wxClientDC dc( this );
	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

	const int w = m_bmpBuffer.GetWidth();
	const int h = m_bmpBuffer.GetHeight();

	if (w == 0 || h == 0)
		return;

	gc->DrawBitmap(m_bmpBuffer, 0.0, 0.0, w, h);
}

unsigned HexTextFrame::GetMinimumWidthForData() const
{
	const unsigned cells = m_renderer.GetCellsPerRow();
	const unsigned gapW = 1;
	const unsigned cellW = (m_renderer.GetCellChars() + gapW)
			* m_director->GetCharSize().GetWidth();

	const unsigned margin = 5;
	return cells * cellW + margin;
}

void HexTextFrame::OnKeyboardInput(wxKeyEvent& event)
{
	std::cout << "HexFrame key: " << event.GetKeyCode() << std::endl;

	// handle movements in the frame because the distance depends on the
	// renderer...
/*	const int keyCode = event.GetKeyCode();

	switch (keyCode)
	{
	}
*/
	// can't handle the key here, maybe someone higher wants it?
	wxPostEvent(GetParent(), event);
}

void HexTextFrame::OnLeftMouseDown(wxMouseEvent& event)
{
	// pass it on until we can deal with it? so as to avoid focus loss
	event.Skip();
}

void HexTextFrame::OnFocusSet(wxFocusEvent& /*event*/)
{
	// TOD: needs to be more persistant than just focus?
	if (HasFocus())
		m_bgColour = wxColour(255, 200, 255);
	else
		m_bgColour = wxColour(255,255,255);

	DataChanged(true); // FIXME force

}

// selection change notification
void HexTextFrame::selectionChanged(const HaxDocument::Selection& selection)
{
	m_selectionRenderer.UpdateSelection(selection);

	const auto regions = m_selectionRenderer.GetPaths();

	// TODO: fix multiregions
	for(auto& region: regions)
	{
		m_selectionPolygon.Clear();

		//std::cout << "Selection polygon: ";
		for (auto& pt: region)
		{
			//std::cout << "(" << pt.x << ", " << pt.y << "), ";
			m_selectionPolygon.Append(new wxPoint(pt.x, pt.y));
		}
		//std::cout << std::endl;
	}

	DataChanged(true); // FIXME force
}
