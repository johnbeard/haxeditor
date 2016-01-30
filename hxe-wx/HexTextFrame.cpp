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
		StringCellRenderer& renderer,
		HaxTextCellFrame& textFrame) :
			wxWindow(parent, id, pos, size),
			m_bmpBuffer(0, 0),
			m_director(director),
			m_bgColour(*wxWHITE),
			m_selectionRenderer(renderer),
			m_textFrame(textFrame)
{
	wxColour c(*wxWHITE);
	SetBackgroundColour(c);

	SetWindowStyleFlag(wxBORDER_SUNKEN);

	m_state.offset = 0;
	m_state.m_margin = wxPoint(3, 0);

	m_caret = new HexCaret(this, m_director->GetCharSize());
	m_caret->Show();

	// listen for the frame's events
	m_textFrame.signal_selectionChanged.connect(
			sigc::mem_fun(this, &HexTextFrame::onSelectionChanged));

	m_textFrame.signal_offsetChanged.connect(
			sigc::mem_fun(this, &HexTextFrame::onOffsetChanged));

	Bind(wxEVT_PAINT, &HexTextFrame::Paint, this);
	Bind(wxEVT_SIZE, &HexTextFrame::OnSize, this);
	Bind(wxEVT_KEY_DOWN, &HexTextFrame::OnKeyboardInput, this);
	m_caret->Bind(wxEVT_KEY_DOWN, &HexTextFrame::OnKeyboardInput, this);
	Bind(wxEVT_LEFT_DOWN, &HexTextFrame::OnLeftMouseDown, this);
	Bind(wxEVT_MOTION, &HexTextFrame::OnMouseMotion, this);
	Bind(wxEVT_SET_FOCUS, &HexTextFrame::OnFocusSet, this);
	Bind(wxEVT_KILL_FOCUS, &HexTextFrame::OnFocusSet, this);
}

HexTextFrame::~HexTextFrame()
{
	Unbind(wxEVT_PAINT, &HexTextFrame::Paint, this);
	Unbind(wxEVT_SIZE, &HexTextFrame::OnSize, this);
	Unbind(wxEVT_KEY_DOWN, &HexTextFrame::OnKeyboardInput, this);
	Unbind(wxEVT_LEFT_DOWN, &HexTextFrame::OnLeftMouseDown, this);
	Unbind(wxEVT_MOTION, &HexTextFrame::OnMouseMotion, this);
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

	const auto yoff = m_selectionRenderer.GetPositionForOffset(m_textFrame.GetPageOffset());

	m_caret->Move((m_textFrame.m_caretPos.charsX + m_textFrame.m_caretPos.gapsX) * charSize.x,
			m_textFrame.m_caretPos.row * charSize.y - yoff);
}

void HexTextFrame::DataChanged(bool force)
{
	moveCaret();

	m_caret->Show(m_textFrame.IsCaretVisible());

	m_pendingState.m_charSize = m_director->GetCharSize();

	// could use a different line spacing
	m_pendingState.m_rows = m_director->GetNumRowsToShow();

	m_pendingState.m_selectionActive = m_selectionPolygon.GetCount();

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

	//std::cout << "Drawing..." << std::endl;

	// draw the selection outline
	//TODO move out to separate window?
	if (m_state.m_selectionActive)
	{
		//std::cout << "Selection poly " << m_selectionPolygon.GetCount() << " items" << std::endl;
		dc.SetBrush(wxColour(0, 100, 255, 50));
		dc.SetPen(wxColour(50, 50, 50, 255));

		// correct for the page offset
		const auto yoff = m_selectionRenderer.GetPositionForOffset(m_textFrame.GetPageOffset());
		dc.DrawPolygon(&m_selectionPolygon, 0, -yoff);
	}

	int yPos = m_state.m_margin.y;
	for ( int y = 0 ; y < m_state.m_rows; ++y )
	{
		int xPos = m_state.m_margin.x;

		const std::string row(m_textFrame.GetRowString(y));

		dc.DrawText(row, xPos, yPos);

		yPos += m_state.m_charSize.y + m_state.m_margin.y;
	}
}

void HexTextFrame::onOffsetChanged(offset_t newOffset)
{
	m_pendingState.offset = newOffset;
	DataChanged(false);
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
	const auto line = m_textFrame.GetMaximumTextWidth();

	const unsigned charW = m_director->GetCharSize().GetWidth();
	const unsigned textW = line.chars * charW + line.gaps * charW;

	const unsigned margin = 5;
	return textW + margin;
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
	// TODO activate, and send signal
	m_textFrame.signal_frameActive.emit(true);

	// find out the click offset from the location
	const auto pos = event.GetPosition();

	const auto offset = m_selectionRenderer.GetOffsetForPosition(pos.x, pos.y);

	m_textFrame.RequestOffsetChange(offset, false);

	// pass it on until we can deal with it? so as to avoid focus loss
	event.Skip();
}

void HexTextFrame::OnMouseMotion(wxMouseEvent& event)
{
	if (event.Dragging())
	{
		const auto pos = event.GetPosition();

		const auto offset = m_selectionRenderer.GetOffsetForPosition(pos.x, pos.y);

		m_textFrame.RequestOffsetChange(offset, true);
	}
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
void HexTextFrame::onSelectionChanged(const HaxDocument::Selection& selection,
		bool active)
{
	if (!active)
	{
		// inactive selection - no polygon to draw...
		m_selectionPolygon.Clear();
	}
	else
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
	}

	DataChanged(true); // FIXME force
}
