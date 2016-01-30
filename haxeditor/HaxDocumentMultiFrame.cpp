/*
 * HaxDocumentMultiFrame.cpp
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#include "HaxDocumentMultiFrame.h"

// ====================
// Multiframe
// ====================

HaxDocumentMultiFrame::HaxDocumentMultiFrame(HaxDocument& doc):
		m_docView(doc)
{
	doc.signal_SelectionChanged.connect(
			sigc::mem_fun(this, &HaxDocumentMultiFrame::onSelectionChanged));

	doc.signal_OffsetChanged.connect(
			sigc::mem_fun(this, &HaxDocumentMultiFrame::onOffsetChanged));

	m_docView.signal_viewStartChanged.connect(sigc::mem_fun(this,
			&HaxDocumentMultiFrame::onViewStartChanged));
}

void HaxDocumentMultiFrame::onOffsetChanged(const offset_t /*newOffset*/)
{
	// all the subframes need to know this
	for (auto& f: m_frames)
	{
		f->SetCaretPosition(m_docView.GetCaretPosition());
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
