/*
 * HaxDocumentMultiFrame.cpp
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#include "HaxDocumentMultiFrame.h"

#include "HaxTextCellFrame.h"

// ====================
// Multiframe
// ====================

HaxDocumentMultiFrame::HaxDocumentMultiFrame(HaxDocument& doc):
		m_docView(doc)
{
	// TODO change this to be dynamic
	const unsigned lineSize = 10 * 8;
	m_docView.SetRowLength(lineSize);

	doc.signal_SelectionChanged.connect(
			sigc::mem_fun(this, &HaxDocumentMultiFrame::onSelectionChanged));

	doc.signal_OffsetChanged.connect(
			sigc::mem_fun(this, &HaxDocumentMultiFrame::onOffsetChanged));

	m_docView.signal_viewStartChanged.connect(sigc::mem_fun(this,
			&HaxDocumentMultiFrame::onViewStartChanged));
}

void HaxDocumentMultiFrame::onOffsetChanged(const offset_t newOffset)
{
	//std::cout << "offset: " << newOffset << std::endl;

	// all the subframes need to know this
	for (auto& f: m_frames)
	{
		f->SetCaretPosition(newOffset);
		f->SetOffset(newOffset);
	}
}

void HaxDocumentMultiFrame::onSelectionChanged(const HaxDocument::Selection& selection)
{
	// all the subframes need to know this
	for (auto& f: m_frames)
	{
		f->ChangeSelection(selection, m_docView.HasActiveSelection());
	}
}

void HaxDocumentMultiFrame::onViewStartChanged(const offset_t newStart)
{
	// update each frame as needed
	for (auto& f: m_frames)
	{
		f->SetOffset(newStart);
	}

	onPageStartChangedInt();
}

void HaxDocumentMultiFrame::onFrameSetsOffset(offset_t offset, bool extendSelection)
{
	//std::cout << "Frame set new offset: " << offset << std::endl;
	m_docView.scrollTo(offset + m_docView.GetPageOffset(), extendSelection, true);
}

HaxTextCellFrame& HaxDocumentMultiFrame::AddNewFrame(std::unique_ptr<StringCellRenderer>& renderer)
{
	renderer->SetWidth(m_docView.GetRowLength());

	// we maintain our own renderers internally
	m_renderer.push_back(std::move(renderer));

	auto frame = std::make_unique<HaxTextCellFrame>(*m_renderer.back().get(), m_docView);

	frame->signal_frameRequestsOffsetChange.connect(
			sigc::mem_fun(this, &HaxDocumentMultiFrame::onFrameSetsOffset));

	m_frames.push_back(std::move(frame));
	return *m_frames.back();
}
