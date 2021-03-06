/*
 * HaxDocumentMultiFrame.cpp
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#include "HaxDocumentMultiFrame.h"

#include "PagedView.h"
#include "SelectionDriver.h"

#include <math.h>

HaxDocumentMultiFrame::HaxDocumentMultiFrame(HaxDocument& doc):
	m_doc(doc),
	m_pagedView(new PagedView()),
	m_selectionDriver(new SelectionDriver(doc.GetSelection()))
{
	// multiframes handle the selections on a byte-wise basis
	m_selectionDriver->SetRoundingUnit(BYTE);

	doc.signal_OffsetChanged.connect(sigc::mem_fun(this, &HaxDocumentMultiFrame::onOffsetChanged));
	doc.signal_SelectionChanged.connect(sigc::mem_fun(this, &HaxDocumentMultiFrame::onSelectionChanged));

	m_pagedView->signal_pageStartChanged.connect(sigc::mem_fun(this,
			&HaxDocumentMultiFrame::onPageStartChanged));

	m_pagedView->SetMaximumOffset(doc.GetDataLength());
}

HaxDocumentMultiFrame::~HaxDocumentMultiFrame()
{}

offset_t HaxDocumentMultiFrame::getRowLength() const
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

offset_t HaxDocumentMultiFrame::GetPageOffset() const
{
	return m_pagedView->GetPageStart();
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

void HaxDocumentMultiFrame::scrollLines(int linesToScrollDown,
		bool extendSelection, bool moveCaret)
{
	if (linesToScrollDown == 0)
		return; //nothing to do

	m_movingCaret = moveCaret;

	const offset_t deltaOffset = std::abs(linesToScrollDown) * getRowLength();

	performDeltaOffset(deltaOffset, linesToScrollDown > 0, extendSelection);
}

void HaxDocumentMultiFrame::scrollTo(offset_t newOffset, bool extendSelection,
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

void HaxDocumentMultiFrame::scrollPageStart(int linesToMovePageBy)
{
	m_pagedView->MovePageStartByLines(linesToMovePageBy);
}

void HaxDocumentMultiFrame::performDeltaOffset(uint64_t delta, bool down,
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

void HaxDocumentMultiFrame::scrollPages(int pagesDown, bool extendSelection)
{
	const auto rowsToScroll = GetNumVisibleRows() - 2; // leave a bit of overlap
	scrollLines(pagesDown * rowsToScroll, extendSelection, false);
}

void HaxDocumentMultiFrame::scrollRight(int unitsRight, bool extendSelection)
{
	std::cout << "Right: " << unitsRight << std::endl;

	m_movingCaret = true;

	// move one nibble
	performDeltaOffset(std::abs(unitsRight) * 4, unitsRight > 0, extendSelection);
}

void HaxDocumentMultiFrame::onOffsetChanged(offset_t newOffset)
{
	std::cout << "Offset change cb: " << newOffset << std::endl;

	// this will fire off page start changes if needed
	m_pagedView->ScrollToKeepOffsetInView(newOffset);

	// update each frame's caret
	if (m_movingCaret)
	{
		for (auto& f: m_frames)
		{
			f->SetCaretPosition(newOffset);
		}

		// update the selection
		m_selectionDriver->onOffsetChanged(newOffset);
	}

	onOffsetChangeInt();
}

void HaxDocumentMultiFrame::onSelectionChanged(const HaxDocument::Selection& selection)
{
	std::cout << "Multiframe selection: " << selection.GetStart() << ":"
			<< selection.GetEnd() << std::endl;

	// all the subframes need to know this
	for (auto& f: m_frames)
	{
		f->ChangeSelection(selection, m_selectionDriver->IsActive());
	}

	onSelectionChangedInt();
}

void HaxDocumentMultiFrame::onPageStartChanged(const PagedView& /*changedView*/)
{
	// don't scroll past the end of the data
	const auto newStart = m_pagedView->GetPageStart();

	// update each frame as needed
	for (auto& f: m_frames)
	{
		f->SetOffset(newStart);
	}

	onPageStartChangedInt();
}
