/*
 * HaxDocumentView.h
 *
 *  Created on: 30 Jan 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_HAXDOCUMENTVIEW_H_
#define HAXEDITOR_HAXDOCUMENTVIEW_H_

#include "haxeditor/HaxDocument.h"

/*!
 * A generalised "view" into a document, which maintains state such as the
 * view start offset, the cursor offset, etc
 */
class HaxDocumentView
{
public:
	/*!
	 * This signal will tell you if this document view has changed its start offset
	 */
	sigc::signal<void, offset_t> signal_viewStartChanged;

protected:
	HaxDocumentView():
		m_cursorOffset(0)
	{}

	virtual ~HaxDocumentView()
	{}

	/*!
	 * Gets the size of the view (number of bytes shown)
	 */
	virtual offset_t getViewSize() const = 0;

	sigc::signal<void, const offset_t> signal_caretMoved;
	// more signals for other changes of document view?

//private:
	// the offset of the cursor (or the start of the cursor if it is longer
	// than one byte
	offset_t m_cursorOffset;
};


#endif /* HAXEDITOR_HAXDOCUMENTVIEW_H_ */
