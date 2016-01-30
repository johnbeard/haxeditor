/*
 * HaxDocumentMultiFrame.h
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#ifndef HAXEDITOR_HAXDOCUMENTMULTIFRAME_H_
#define HAXEDITOR_HAXDOCUMENTMULTIFRAME_H_

#include "HaxFrame.h"
#include "HaxPagedDocumentView.h"

#include <memory>
#include <vector>

/*!
 * The HaxDocumentMultiFrame is the core of the editor - if shows a set of
 * HaxFrames, which each contain a view of the data. These frames scroll
 * together - a line in one means a line in the others, and are therefore
 * driven from the same HaxPagedDocumentView
 */
class HaxDocumentMultiFrame
{
public:
	HaxDocumentMultiFrame(HaxDocument& doc);
protected:
	virtual ~HaxDocumentMultiFrame()
	{}

	// todo encapsulate
	std::vector<HaxFrame*> m_frames;

	// TODO and this?
	HaxPagedDocumentView m_docView;

private:

	/*!
	 * Handle a new selection
	 * Called by the selection provider - probably the HaxDocument
	 * @param selection - the new selection
	 */
	void onSelectionChanged(const HaxDocument::Selection& selection);

	/*!
	 * Called when the paged view start changes
	 */
	virtual void onViewStartChanged(const offset_t changedView);

	/*!
	 * Implement this to respond to page start (not necessarily document offset)
	 * changes
	 */
	virtual void onPageStartChangedInt() = 0;

	void onOffsetChanged(const offset_t /*newOffset*/);
};

#endif /* HAXEDITOR_HAXDOCUMENTMULTIFRAME_H_ */
