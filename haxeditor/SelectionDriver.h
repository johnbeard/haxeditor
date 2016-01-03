/*
 * SelectionDriver.h
 *
 *  Created on: 16 Nov 2015
 *      Author: john
 */

#ifndef HAXEDITOR_SELECTIONDRIVER_H_
#define HAXEDITOR_SELECTIONDRIVER_H_

#include "haxeditor/haxeditor.h"
#include "utils/MathsUtils.h"

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
		m_selection(selection)
	{
	}

	bool IsActive() const
	{
		return m_selectionActive;
	}

	void SetActive(bool active)
	{
		if (active != m_selectionActive)
		{
			m_selectionActive = active;

			if (!active)
			{
				// reset (will notify)
				m_selection.SetRange(0, 0);
			}
		}
	}

	/*!
	 * Set the active "end" of the selection (left or right)
	 * @param left
	 */
	void SetActiveType(ActiveType type)
	{
		m_activeType = type;
	}

	void ResetOffset(offset_t offset);

	/*!
	 * Set the rounding unit of the selection driver - this is the "blocks"
	 * that the selection will snap to. For example, if the unit is "8", a
	 * requested selection of 1:6 will actually be 0:8, and 1:8 will actually
	 * be 0:16.
	 */
	void SetRoundingUnit(unsigned unit)
	{
		m_roundingUnit = unit;
	}

	/*!
	 * Handle an offset change.
	 *
	 * If we are not active, we don't care. If we are active, the we need to
	 * move one end of the selection if appropriate.
	 *
	 * @param newOffset
	 */
	void onOffsetChanged(offset_t newOffset);

private:

	// is the client currently acting on the selection?
	bool m_selectionActive = false;

	// round the ends of the selection to multiples of these
	unsigned m_roundingUnit = 1;

	// which end is growing?
	ActiveType m_activeType = Left;

	// the actual selection is provided externally
	HaxDocument::Selection& m_selection;
};


#endif /* HAXEDITOR_SELECTIONDRIVER_H_ */
