/*
 * HaxDocumentMultiFrame.cpp
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#include "HaxDocumentMultiFrame.h"

#include <math.h>

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
	performDeltaOffset(unitsRight > 0, unitsRight);
}


