/*
 * HaxPagedDocumentView.cpp
 *
 *  Created on: 30 Jan 2016
 *      Author: John Beard
 */

#include "HaxPagedDocumentView.h"

#include "PagedView.h"
#include "SelectionDriver.h"

#include <math.h>
#include <iostream>

HaxPagedDocumentView::HaxPagedDocumentView(HaxDocument& doc):
	m_doc(doc),
	m_pagedView(new PagedView()),
	m_selectionDriver(new SelectionDriver(doc.GetSelection()))
{
	// multiframes handle the selections on a byte-wise basis
	m_selectionDriver->SetRoundingUnit(BYTE);

	doc.signal_OffsetChanged.connect(
			sigc::mem_fun(this, &HaxPagedDocumentView::onOffsetChanged));

	m_pagedView->signal_pageStartChanged.connect(
			sigc::mem_fun(this, &HaxPagedDocumentView::onPageStartChanged));

	m_pagedView->SetMaximumOffset(doc.GetDataLength());
}

HaxPagedDocumentView::~HaxPagedDocumentView()
{}

offset_t HaxPagedDocumentView::GetRowLength() const
{
	return m_pagedView->GetRowLength();
}

void HaxPagedDocumentView::SetRowLength(unsigned length)
{
	m_pagedView->SetRowLength(length);
}

void HaxPagedDocumentView::SetNumVisibleRows(unsigned rowsToShow)
{
	m_rows = rowsToShow;

	// update the paged view wit ha smaller size to leave row "free" at the
	// bottom that is not part of the "visible area" - used to allow the last
	// row to be partically hidden
	m_pagedView->SetPageSize(getViewSize() - GetRowLength());
}


uint64_t HaxPagedDocumentView::getTotalNumRows() const
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

offset_t HaxPagedDocumentView::getMaximumOffsetRow() const
{
	const uint64_t pageRows = GetNumVisibleRows();
	const uint64_t totalRows = getTotalNumRows();

	if (pageRows > totalRows)
		return 0;

	return totalRows - pageRows;
}

offset_t HaxPagedDocumentView::GetPageOffset() const
{
	return m_pagedView->GetPageStart();
}


void HaxPagedDocumentView::scrollToStart()
{
	m_doc.SetOffset(0);
}

void HaxPagedDocumentView::scrollToEnd()
{
//	m_rowOffset = getMaximumOffsetRow();
//	updateOffset();

	m_doc.SetOffset(m_doc.GetDataLength());
}

void HaxPagedDocumentView::scrollLines(int linesToScrollDown,
		bool extendSelection, bool moveCaret)
{
	if (linesToScrollDown == 0)
		return; //nothing to do

	m_movingCaret = moveCaret;

	const offset_t deltaOffset = std::abs(linesToScrollDown) * GetRowLength();

	performDeltaOffset(deltaOffset, linesToScrollDown > 0, extendSelection);
}

void HaxPagedDocumentView::scrollTo(offset_t newOffset, bool extendSelection,
		bool moveCaret)
{
	// transform to a delta move
	// TOOO reverse this?

	m_movingCaret = moveCaret;

	const auto currOffset = m_doc.GetOffset();
	const bool down = newOffset > currOffset;

	const auto delta = (down) ? (newOffset - currOffset) : (currOffset - newOffset);

	performDeltaOffset(delta, down, extendSelection);
}

void HaxPagedDocumentView::scrollPageStart(int linesToMovePageBy)
{
	m_pagedView->MovePageStartByLines(linesToMovePageBy);
}

void HaxPagedDocumentView::performDeltaOffset(offset_t delta, bool down,
		bool extendSelection)
{
	const auto currOffset = m_doc.GetOffset();
	// new selection
	if (extendSelection && !m_selectionDriver->IsActive())
	{
		// selection starts (and ends) at the current offset
		m_selectionDriver->ResetOffset(currOffset);

		// which end are we moving?
		m_selectionDriver->SetActiveType(down
				? SelectionDriver::Right : SelectionDriver::Left);
	}

	// activate/deactivate the selection
	m_selectionDriver->SetActive(extendSelection);

	auto newOffset = currOffset;
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

void HaxPagedDocumentView::scrollPages(int pagesDown, bool extendSelection)
{
	const auto rowsToScroll = GetNumVisibleRows() - 2; // leave a bit of overlap
	scrollLines(pagesDown * rowsToScroll, extendSelection, false);
}

void HaxPagedDocumentView::scrollRight(int unitsRight, bool extendSelection)
{
	std::cout << "Right: " << unitsRight << std::endl;

	m_movingCaret = true;

	// move one nibble
	performDeltaOffset(std::abs(unitsRight) * 4, unitsRight > 0, extendSelection);
}

bool HaxPagedDocumentView::HasActiveSelection() const
{
	return m_selectionDriver->IsActive();
}

void HaxPagedDocumentView::onOffsetChanged(offset_t newOffset)
{
	// this will fire off page start changes if needed
	m_pagedView->ScrollToKeepOffsetInView(newOffset);

	// update each frame's caret
	if (m_movingCaret)
	{
		// update the selection
		m_selectionDriver->onOffsetChanged(newOffset);
	}
}

void HaxPagedDocumentView::onPageStartChanged(const PagedView& newStart)
{
	// we don't need to handle anything here, but this drives the basic
	// document view signal which is what clients will be listening for
	signal_viewStartChanged.emit(newStart.GetPageStart());
}


