/*
 * HaxDocumentMultiFrame.cpp
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#include "HaxDocumentMultiFrame.h"

#include "PagedView.h"

#include <math.h>

HaxDocumentMultiFrame::HaxDocumentMultiFrame(HaxDocument& doc):
	m_doc(doc),
	m_rowOffset(0),
	m_rows(0),
	m_pagedView(std::unique_ptr<PagedView>(new PagedView()))
{
	// connect the signals
	doc.signal_OffsetChanged.connect(sigc::mem_fun(this, &HaxDocumentMultiFrame::onOffsetChanged));
	doc.signal_SelectionChanged.connect(sigc::mem_fun(this, &HaxDocumentMultiFrame::onSelectionChanged));
}

HaxDocumentMultiFrame::~HaxDocumentMultiFrame()
{}

unsigned HaxDocumentMultiFrame::getRowLength() const
{
	return m_pagedView->GetRowLength();
}

void HaxDocumentMultiFrame::setRowLength(unsigned length)
{
	m_pagedView->SetRowLength(length);
}

void HaxDocumentMultiFrame::setNumVisibleRows(unsigned rowsToShow)
{
	m_rows = rowsToShow;

	// update the paged view wit ha smaller size to leave row "free" at the
	// bottom that is not part of the "visible area" - used to allow the last
	// row to be partically hidden
	m_pagedView->SetPageSize(getViewSize() - getRowLength());
}


uint64_t HaxDocumentMultiFrame::getTotalNumRows() const
{
	const uint64_t dataSize = m_doc.GetDataLength();
	const unsigned lineSize = 10;

	uint64_t dataRows = dataSize / lineSize;
	const unsigned lastRowCount = dataSize % lineSize;

	if (lastRowCount)
		dataRows += 1;

	// one more row because we draw the last row off the bottom edge
	dataRows += 1;

	return dataRows;
}

uint64_t HaxDocumentMultiFrame::getMaximumOffsetRow() const
{
	const uint64_t pageRows = GetNumVisibleRows();
	const uint64_t totalRows = getTotalNumRows();

	if (pageRows > totalRows)
		return 0;

	return totalRows - pageRows;
}


void HaxDocumentMultiFrame::scrollToStart()
{
	m_doc.SetOffset(0);
}

void HaxDocumentMultiFrame::scrollToEnd()
{
//	m_rowOffset = getMaximumOffsetRow();
//	updateOffset();

	m_doc.SetOffset(m_doc.GetDataLength());
}

void HaxDocumentMultiFrame::scrollLines(int linesToScrollDown)
{
	if (linesToScrollDown == 0)
		return; //nothing to do

	const uint64_t deltaOffset = std::abs(linesToScrollDown) * getRowLength();

	performDeltaOffset(deltaOffset, linesToScrollDown > 0);
}

void HaxDocumentMultiFrame::performDeltaOffset(uint64_t delta, bool down)
{
	uint64_t newOffset = m_doc.GetOffset();
	if (down) // down
	{
		newOffset = std::min(m_doc.GetDataLength(), newOffset + delta);
	}
	else
	{
		if (delta > newOffset)
			newOffset = 0;
		else
			newOffset -= delta;
	}

	m_doc.SetOffset(newOffset);
}

void HaxDocumentMultiFrame::scrollPages(int pagesDown)
{
	scrollLines(pagesDown * (GetNumVisibleRows() - 2)); // leave a bit of overlap
}

void HaxDocumentMultiFrame::scrollRight(int unitsRight)
{
	std::cout << "Right: " << unitsRight << std::endl;

	// move one nibble
	performDeltaOffset(std::abs(unitsRight) * 4, unitsRight > 0);
}

void HaxDocumentMultiFrame::onOffsetChanged(offset_t newOffset)
{
	std::cout << "Offset change cb: " << newOffset << std::endl;

	if (m_pagedView->ScrollToKeepOffsetInView(newOffset))
	{
		const offset_t newStart = m_pagedView->GetPageStart();

		// update each frame as needed
		for (auto& f: m_frames)
		{
			f->SetOffset(newStart);
		}
	}

	// update each frame's caret
	for (auto& f: m_frames)
	{
		f->SetCaretPosition(newOffset);
	}

	onOffsetChangeInt();
}

void HaxDocumentMultiFrame::onSelectionChanged(const HaxDocument::Selection& selection)
{
	std::cout << "Multiframe selection: " << selection.GetStart() << ":"
			<< selection.GetEnd() << std::endl;

	onSelectionChangedInt();
}
