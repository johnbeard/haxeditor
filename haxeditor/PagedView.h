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
	PagedView():
		pageStart(0),
		pageSize(0),
		rowLength(0)
	{}

	virtual ~PagedView()
	{}

	bool ScrollToKeepOffsetInView(offset_t newOffset)
	{
		if (!rowLength)
			return false;

		bool moved = false;
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

			// TODO: notify..
			pageStart = newStart;
			moved = true;
		}

		return moved;
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

	unsigned GetRowLength() const
	{
		return rowLength;
	}

	void SetRowLength(unsigned length)
	{
		rowLength = length;
	}

private:
	// Where the page starts
	offset_t pageStart;

	// amount of data shown in a single page
	offset_t pageSize;

	// the length of a single row of data
	unsigned rowLength;
};



#endif /* HAXEDITOR_PAGEDVIEW_H_ */
