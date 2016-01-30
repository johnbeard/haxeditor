/*
 * HaxPagedDocumentView.h
 *
 *  Created on: 30 Jan 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_HAXPAGEDDOCUMENTVIEW_H_
#define HAXEDITOR_HAXPAGEDDOCUMENTVIEW_H_

#include "HaxDocumentView.h"

#include <memory>

/*!
 * A HaxPagedDocumentView is a view into a document that acts on "pages"
 * and encapsulated movement and selection within the document with basic
 * movements like l/r, up/down, page up/down, etc
 */
class HaxPagedDocumentView: public HaxDocumentView
{
public:

	HaxPagedDocumentView(HaxDocument& doc);
	~HaxPagedDocumentView();

	// ========================================================================
	// Functions to control the size and layout of the view
	// ========================================================================
	void SetRowLength(unsigned rowLength);
	offset_t GetRowLength() const;

	void SetNumVisibleRows(unsigned rowsToShow);

	// ========================================================================
	// Functions to manipulate the caret and selection in this view
	// ========================================================================

	void scrollTo(offset_t newOffset, bool extendSelection, bool moveCaret);
	void scrollToStart();
	void scrollToEnd();
	void scrollLines(int linesToScrollDown, bool extendSelection, bool moveCaret);
	void scrollPages(int pagesDown, bool extendSelection);
	void scrollRight(int unitsRight, bool extendSelection);
	void scrollPageStart(int linesToMovePage);

	unsigned GetNumVisibleRows() const
	{
		return m_rows;
	}

	/*!
	 * @return the offset at the start of the page
	 */
	offset_t GetPageOffset() const;

	/*!
	 * Get the caret position in the document
	 */
	offset_t GetCaretPosition() const
	{
		return m_doc.GetOffset();
	}

	bool HasActiveSelection() const;

protected:

	offset_t getViewSize() const override
	{
		return m_rows * GetRowLength();
	}

	/*!
	 * Get the total length of the document, in rows, as rendered at the current
	 * width.
	 * @return
	 */
	uint64_t getTotalNumRows() const;

	/*!
	 * Get the row index of the maximum row you can scroll too (so that you
	 * don't scroll into a mostly black page at the end
	 */
	offset_t getMaximumOffsetRow() const;

	/*!
	 * The document that we are wrapping
	 */
	HaxDocument& m_doc;

private:

	virtual void performDeltaOffset(offset_t offset, bool down,
			bool extendSelection);

	/*!
	 * Handle an offset change
	 * Called by the offset provider - probably the HaxDocument
	 * @param newOffset new offset - guaranteed to be in the document
	 */
	void onOffsetChanged(offset_t newOffset);

	void onPageStartChanged(const class PagedView& changedView);

	uint64_t m_rowOffset = 0;

	unsigned m_rows = 0;

	// logic for paging
	std::unique_ptr<class PagedView> m_pagedView;

	// selection wrapper/driver
	std::unique_ptr<class SelectionDriver> m_selectionDriver;

	// are we moving the caret?
	bool m_movingCaret = false;
};



#endif /* HAXEDITOR_HAXPAGEDDOCUMENTVIEW_H_ */
