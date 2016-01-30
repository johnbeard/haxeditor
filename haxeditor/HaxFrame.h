/*
 * HaxFrame.h
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#ifndef HAXEDITOR_HAXFRAME_H_
#define HAXEDITOR_HAXFRAME_H_

#include "haxeditor/HaxDocument.h"

/*!
 * The main type of data view in haxeditor: a frame filled will some sort
 * of cells, arranged in rows
 */
class HaxFrame
{
public:
	HaxFrame();

	virtual ~HaxFrame()
	{}

	virtual void SetCaretPosition(offset_t newPos) = 0;

	/*!
	 * Modify the selection that this frame shows
	 * @param selection the new selection
	 */
	void ChangeSelection(const HaxDocument::Selection& selection,
			bool active)
	{
		// we don't do anything, but clients might be listening for the frame
		// to change selection
		signal_selectionChanged.emit(selection, active);
	}

	void SetOffset(offset_t newOffset)
	{
		signal_offsetChanged.emit(newOffset);
	}

	/*!
	 * Call this if an action on a frame is driving the offset (eg a click).
	 *
	 * This is releative to the frame start
	 * @param requestedOffset
	 * @param extendSelection
	 */
	void RequestOffsetChange(offset_t requestedOffset, bool extendSelection)
	{
		signal_frameRequestsOffsetChange.emit(requestedOffset, extendSelection);
	}

	// signalled when a click or similar is driving the offset
	sigc::signal<void, offset_t, bool> signal_frameRequestsOffsetChange;

	// signalled when the frame start moves (clients probably need redraws)
	sigc::signal<void, offset_t> signal_offsetChanged;

	sigc::signal<void, const HaxDocument::Selection&, bool> signal_selectionChanged;

	// signalled when a frame has been made active
	sigc::signal<void, bool> signal_frameActive;


	bool IsCaretVisible() const
	{
		return m_caretVisible;
	}

	void SetCaretVisible(bool visible)
	{
		m_caretVisible = visible;
	}

private:

	bool m_caretVisible;
};



#endif /* HAXEDITOR_HAXFRAME_H_ */
