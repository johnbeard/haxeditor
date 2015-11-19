/*
 * HaxFrame.cpp
 *
 *  Created on: 19 Nov 2015
 *      Author: John Beard
 */

#include "HaxFrame.h"

HaxFrame::HaxFrame(HaxDocument::Selection& selection):
	m_caretVisible(true)
{
	// when the selection changes, we might want to draw it?
	selection.signal_changed.connect(sigc::mem_fun(this,
			&HaxFrame::selectionChanged));
}


