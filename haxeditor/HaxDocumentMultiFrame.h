/*
 * HaxDocumentMultiFrame.h
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#ifndef HAXEDITOR_HAXDOCUMENTMULTIFRAME_H_
#define HAXEDITOR_HAXDOCUMENTMULTIFRAME_H_

#include "haxeditor.h"
#include "HaxFrame.h"

#include <memory>

/*!
 * A generalised "view" into a document, which maintains state such as the
 * view start offset, the cursor offset, etc
 */
class HaxDocumentView
{
protected:
	HaxDocumentView():
		m_startOffset(0),
		m_cursorOffset(0)
	{}

	virtual ~HaxDocumentView()
	{}

	offset_t getStartOffset() const
	{
		return m_startOffset;
	}

	void setStartOffset(offset_t newStart)
	{
		m_startOffset = newStart;
	}

	/*!
	 * Gets the size of the view (number of bytes shown)
	 */
	virtual offset_t getViewSize() const = 0;

//private:
	offset_t m_startOffset;

	// the offset of the cursor (or the start of the cursor if it is longer
	// than one byte
	offset_t m_cursorOffset;
};

/*
 * The HaxDocumentMultiFrame is the core of the editor - if shows a set of
 * HaxFrames, which each contain a view of the data. These frames scroll
 * together - a line in one means a line in the others
 */
class HaxDocumentMultiFrame: public HaxDocumentView
{
public:

	HaxDocumentMultiFrame(HaxDocument& doc):
		m_doc(doc),
		m_rowOffset(0),
		m_rowLength(0),
		m_rows(0)
	{
		doc.signal_OffsetChanged.connect(sigc::mem_fun(this, &HaxDocumentMultiFrame::onOffsetChanged));
	}

	virtual ~HaxDocumentMultiFrame()
	{}

	void scrollToStart();
	void scrollToEnd();
	void scrollLines(int linesToScrollDown);
	void scrollPages(int pagesDown);
	void scrollRight(int unitsRight);

	void SetOffset(uint64_t newOffset)
	{
		m_rowOffset = newOffset;
	}

	uint64_t GetRowOffset() const
	{
		return m_rowOffset;
	}

	unsigned GetNumVisibleRows() const
	{
		return m_rows;
	}

protected:

	offset_t getViewSize() const override
	{
		return m_rows * getRowLength();
	}

	void setNumVisibleRows(unsigned rowsToShow)
	{
		m_rows = rowsToShow;
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
	uint64_t getMaximumOffsetRow() const;

	void setRowLength(unsigned rowLength)
	{
		m_rowLength = rowLength;
	}

	unsigned getRowLength() const
	{
		return m_rowLength;
	}

	HaxDocument& m_doc;

private:

	virtual void performDeltaOffset(uint64_t offset, bool down);

	/*!
	 * Implement to handle an offset change
	 * Called by the offset provider - probably the HaxDocument
	 * @param newOffset new offset - guaranteed to be in the document
	 */
	virtual void onOffsetChanged(uint64_t newOffset) = 0;

	uint64_t m_rowOffset;

	unsigned m_rowLength;
	unsigned m_rows;
};

#endif /* HAXEDITOR_HAXDOCUMENTMULTIFRAME_H_ */
