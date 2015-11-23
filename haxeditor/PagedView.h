/*
 * PagedView.h
 *
 *  Created on: 5 Nov 2015
 *      Author: john
 */

#ifndef HAXEDITOR_PAGEDVIEW_H_
#define HAXEDITOR_PAGEDVIEW_H_

#include "haxeditor.h"

/*!
 * PagedView is used to describe a view which can show a page of information
 * at once. It provides functions for scrolling around in the view and so on.
 */
class PagedView
{
public:
	PagedView()
	{}

	virtual ~PagedView()
	{}

	void ScrollToKeepOffsetInView(offset_t newOffset)
	{
		if (!rowLength)
			return;

		// do we need to move the whole view, or just the caret?
		const bool movedOffTop = newOffset < pageStart;
		const bool movedOffBottom = newOffset >= (pageStart + pageSize);

		if (movedOffBottom || movedOffTop)
		{
			// set the new start offset such that the offset is on the top row
			offset_t newStart = rowLength * (newOffset / rowLength);

			if (movedOffBottom)
			{
				// moved off the bottom, the new offset should be at the bottom of
				// the view
				if (newStart < pageSize)
				{
					// now space above, move as far as possible
					newStart = 0;
				}
				else
				{
					// move up one page
					// TODO magic 2
					newStart -= pageSize - rowLength;
				}
			}

			changePageStart(newStart);
		}
	}

	offset_t GetPageSize() const
	{
		return pageSize;
	}

	offset_t GetPageStart() const
	{
		return pageStart;
	}

	void SetPageSize(offset_t newSize)
	{
		pageSize = newSize;
	}

	void SetMaximumOffset(offset_t newMax)
	{
		m_maximumOffset = newMax;

		// TODO: relocate on a smaller maximum?
	}

	unsigned GetRowLength() const
	{
		return rowLength;
	}

	void SetRowLength(unsigned length)
	{
		rowLength = length;
	}

	void MovePageStartByLines(int linesToMovePageBy)
	{
		auto pageOff = pageStart;
		const auto delta = std::abs(linesToMovePageBy) * GetRowLength();

		if (linesToMovePageBy < 0)
		{
			if (delta > pageStart)
				pageOff = 0;
			else
				pageOff -= delta;
		}
		else
		{
			pageOff += delta;
		}

		changePageStart(pageOff);
	}

	/*!
	 * This signal will tell you if a PagedView has changed its start offset
	 */
	sigc::signal<void, const PagedView&> signal_pageStartChanged;

private:

	void changePageStart(offset_t newStart)
	{
		if (m_maximumOffset)
		{
			// see if we hit the max
			if (newStart + pageSize > m_maximumOffset)
			{
				if (pageSize > m_maximumOffset)
					newStart = 0;
				else
					newStart = m_maximumOffset - pageSize;
			}
		}

		// change if needed
		if (pageStart != newStart)
		{
			pageStart = newStart;

			signal_pageStartChanged.emit(*this);
		}
	}

	// Where the page starts
	offset_t pageStart = 0;

	// amount of data shown in a single page
	offset_t pageSize = 0;

	// maximum offset this pagedview will show
	// 0 for no limit
	offset_t m_maximumOffset = 0;

	// the length of a single row of data
	unsigned rowLength = 0;
};



#endif /* HAXEDITOR_PAGEDVIEW_H_ */
