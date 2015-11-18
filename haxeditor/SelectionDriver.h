/*
 * SelectionDriver.h
 *
 *  Created on: 16 Nov 2015
 *      Author: john
 */

#ifndef HAXEDITOR_SELECTIONDRIVER_H_
#define HAXEDITOR_SELECTIONDRIVER_H_

#include "haxeditor/haxeditor.h"

#include <memory>

class SelectionDriver
{
public:

	/*!
	 * This enum dictates how the selection driver will respond to offset
	 * changes. For example, a "Left" type will move the left end only.
	 */
	enum ActiveType
	{
		Left,		//!< Left
		Right		//!< Right
	};

	SelectionDriver(HaxDocument::Selection& selection):
		m_selectionActive(false),
		m_activeType(Left),
		m_selection(selection)
	{
	}

	bool IsActive() const
	{
		return m_selectionActive;
	}

	void SetActive(bool active)
	{
		m_selectionActive = active;
	}

	/*!
	 * Set the active "end" of the selection (left or right)
	 * @param left
	 */
	void SetActiveType(ActiveType type)
	{
		m_activeType = type;
	}

	/*!
	 * Handle an offset change.
	 *
	 * If we are not active, we don't care. If we are active, the we need to
	 * move one end of the selection if appropriate.
	 *
	 * @param newOffset
	 */
	void onOffsetChanged(offset_t newOffset)
	{
		if (!IsActive())
			return;

		const auto start = m_selection.GetStart();
		const auto end = m_selection.GetEnd();

		auto endToKeep = (m_activeType == Right) ? start : end;

		m_selection.SetRange(newOffset, endToKeep);
	}

private:

	// is the client currently acting on the selection?
	bool m_selectionActive;

	// which end is growing?
	ActiveType m_activeType;

	// the actual selection is provided externally
	HaxDocument::Selection& m_selection;
};


#endif /* HAXEDITOR_SELECTIONDRIVER_H_ */
