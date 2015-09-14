
#include "HugeScrollBar.h"

#include <iostream>

#define _DEBUG_SCROLL_

wxHugeScrollBar::wxHugeScrollBar(wxScrollBar* m_scrollbar):
		m_range(0),
		m_thumb(0),
		m_scrollbar(m_scrollbar)
{
	m_scrollbar->Bind(wxEVT_SCROLL_TOP, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Bind(wxEVT_SCROLL_BOTTOM, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Bind(wxEVT_SCROLL_LINEUP, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Bind(wxEVT_SCROLL_LINEDOWN, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Bind(wxEVT_SCROLL_PAGEUP, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Bind(wxEVT_SCROLL_PAGEDOWN, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Bind(wxEVT_SCROLL_THUMBTRACK, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Bind(wxEVT_SCROLL_THUMBRELEASE, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Bind(wxEVT_SCROLL_CHANGED, &wxHugeScrollBar::OnOffsetScroll, this);
}

wxHugeScrollBar::~wxHugeScrollBar()
{
	m_scrollbar->Unbind(wxEVT_SCROLL_TOP, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Unbind(wxEVT_SCROLL_BOTTOM, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Unbind(wxEVT_SCROLL_LINEUP, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Unbind(wxEVT_SCROLL_LINEDOWN, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Unbind(wxEVT_SCROLL_PAGEUP, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Unbind(wxEVT_SCROLL_PAGEDOWN, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Unbind(wxEVT_SCROLL_THUMBTRACK, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Unbind(wxEVT_SCROLL_THUMBRELEASE, &wxHugeScrollBar::OnOffsetScroll, this);
	m_scrollbar->Unbind(wxEVT_SCROLL_CHANGED, &wxHugeScrollBar::OnOffsetScroll, this);
}

void wxHugeScrollBar::SetThumbPosition(int64_t setpos)
{
#ifdef _DEBUG_SCROLL_
	std::cout << "SetThumbPosition()" << setpos << std::endl;
#endif
	m_thumb = setpos;
	if (m_range <= 2147483647)
	{
		m_scrollbar->SetThumbPosition(setpos);
	}
	else
	{
#ifdef _DEBUG_SCROLL_
		std::cout << "m_Range: " << m_range << std::endl;
		std::cout << "SetThumbPositionx(): " << static_cast<int>(setpos*(2147483648.0/m_range)) << std::endl;
#endif
		m_scrollbar->SetThumbPosition(
				static_cast<int>(setpos * (2147483648.0 / m_range)));
	}
}

void wxHugeScrollBar::SetScrollbar(int64_t Current_Position, int page_x,
		int64_t new_range, int pagesize, bool repaint)
{
	m_range = new_range;
	if (new_range <= 2147483647)
	{ //if representable with 32 bit
		m_scrollbar->SetScrollbar(Current_Position, page_x, new_range, pagesize,
				repaint);
	}
	else
	{
#ifdef _DEBUG_SCROLL_
		std::cout << "new_range " << new_range << std::endl;
		std::cout << "Current_Position :" << (Current_Position*(2147483647/new_range)) << std::endl;
#endif
		m_scrollbar->SetScrollbar((Current_Position * (2147483647 / new_range)),
				page_x, 2147483647, pagesize, repaint);
	}
	SetThumbPosition(Current_Position);
}

void wxHugeScrollBar::OnOffsetScroll(wxScrollEvent& event)
{
	if (m_range <= 2147483647)
	{
		m_thumb = event.GetPosition();
	}
	else
	{	//64bit mode
		int64_t here = event.GetPosition();
		if (here == 2147483646)	//if maximum set
			m_thumb = m_range - 1;//than give maximum m_thumb which is -1 from range
		else
			m_thumb = static_cast<int64_t>(here * (m_range / 2147483647.0));
	}
	wxYieldIfNeeded();

#ifdef _DEBUG_SCROLL_
	if(event.GetEventType() == wxEVT_SCROLL_CHANGED)
		std::cout << "wxEVT_SCROLL_CHANGED " << m_thumb << std::endl;
	if(event.GetEventType() == wxEVT_SCROLL_THUMBTRACK)
		std::cout << "wxEVT_SCROLL_THUMBTRACK" << m_thumb << std::endl;
	if(event.GetEventType() == wxEVT_SCROLL_THUMBRELEASE)
		std::cout << "wxEVT_SCROLL_THUMBRELEASE"  << std::endl;
	if( event.GetEventType() == wxEVT_SCROLL_LINEDOWN )
		std::cout << "wxEVT_SCROLL_LINEDOWN"  << std::endl;
	if( event.GetEventType() == wxEVT_SCROLL_LINEUP )
		std::cout << "wxEVT_SCROLL_LINEUP" << std::endl;
	if( event.GetEventType() == wxEVT_SCROLL_PAGEUP )
		std::cout << "wxEVT_SCROLL_PAGEUP" << std::endl;
	if( event.GetEventType() == wxEVT_SCROLL_PAGEDOWN )
		std::cout << "wxEVT_SCROLL_PAGEDOWN" << std::endl;
	if( event.GetEventType() == wxEVT_SCROLLWIN_LINEUP )
		std::cout << "wxEVT_SCROLLWIN_LINEUP" << std::endl;
	if( event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN )
		std::cout << "wxEVT_SCROLLWIN_LINEDOWN" << std::endl;
#endif

	event.Skip();
}
