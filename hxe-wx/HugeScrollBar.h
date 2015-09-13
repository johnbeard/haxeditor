/*
 * HugeScrollBar.h
 *
 *  Created on: 13 Sep 2015
 *      Author: john
 */

#ifndef HXE_WX_HUGESCROLLBAR_H_
#define HXE_WX_HUGESCROLLBAR_H_

#include <stdint.h>

#include "wx/scrolbar.h"

/*!
 * //64bit wrapper for wxScrollbar
 */
class wxHugeScrollBar: public wxEvtHandler
{
public:
	wxHugeScrollBar(wxScrollBar* m_scrollbar);
	~wxHugeScrollBar();

	void Enable(bool en)
	{
		m_scrollbar->Enable(en);
	}

	wxSize GetSize(void)
	{
		return m_scrollbar->GetSize();
	}

	uint64_t GetRange(void)
	{
		return m_range;
	}

	uint64_t GetThumbPosition(void)
	{
		return m_thumb;
	}

	void SetThumbPosition(int64_t setpos);
	void SetScrollbar(int64_t Current_Position, int page_x, int64_t new_range,
			int pagesize, bool repaint = true);
	void OnOffsetScroll(wxScrollEvent& event);

private:
	uint64_t m_range;
	uint64_t m_thumb;
	wxScrollBar *m_scrollbar;
};


#endif /* HXE_WX_HUGESCROLLBAR_H_ */
