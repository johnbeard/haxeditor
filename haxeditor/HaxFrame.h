/*
 * HaxFrame.h
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#ifndef HAXEDITOR_HAXFRAME_H_
#define HAXEDITOR_HAXFRAME_H_

#include <sigc++-2.0/sigc++/sigc++.h>

#include "haxeditor/haxeditor.h"

/*!
 * The main type of data view in haxeditor: a frame filled will some sort
 * of cells, arranged in rows
 */
class HaxFrame
{
public:
	HaxFrame(HaxDocument::Selection& selection);

	virtual ~HaxFrame()
	{}

	virtual void SetOffset(uint64_t newOffset) = 0;

	virtual unsigned GetMinimumWidthForData() const = 0;

	virtual void SetCaretPosition(offset_t newPos) = 0;

	virtual void moveCaret() = 0;

	// signalled when a click or similar is driving the offset
	sigc::signal<void, offset_t, bool> signal_offsetChanged;

	// signalled when a frame has been made active
	sigc::signal<void, bool> signal_frameActive;

protected:

	bool isCaretVisible() const
	{
		return m_caretVisible;
	}

	void setCaretVisible(bool visible)
	{
		m_caretVisible = visible;
	}

private:

	/*!
	 * Implement to handle a selection change
	 * @param selection the new selection
	 */
	virtual void selectionChanged(const HaxDocument::Selection& selection) = 0;

	bool m_caretVisible;
};



#endif /* HAXEDITOR_HAXFRAME_H_ */
