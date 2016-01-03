/*
 * SelectionDriver.cpp
 *
 *  Created on: 25 Dec 2015
 *      Author: John Beard
 */

#include "SelectionDriver.h"

void SelectionDriver::ResetOffset(offset_t offset)
{
	auto start = offset;
	auto end = offset;

	if (m_roundingUnit > 0)
	{
		start = Hax::Maths::RoundDown(offset, m_roundingUnit);
		end = Hax::Maths::RoundUp(offset, m_roundingUnit);

		// include the next "block" if we are on the fence
		if (offset % m_roundingUnit == 0)
		{
			end += m_roundingUnit;
		}
	}

	m_selection.SetRange(start, end);
}

void SelectionDriver::onOffsetChanged(offset_t newOffset)
{
	if (!IsActive())
		return;

	auto start = m_selection.GetStart();
	auto end = m_selection.GetEnd();

	// look out for "inversions" which mean we swap the sense of the
	// selection (eg moving the left edge, but we move rightwards, past the
	// existing right edge, then we _become_ the right edge!
	auto inverted = false;
	if (m_activeType == Left && newOffset >= end)
	{
		m_activeType = Right;
		inverted = true;
	}
	else if (m_activeType == Right && newOffset <= start)
	{
		m_activeType = Left;
		inverted = true;
	}

	if (m_activeType == Left)
	{
		end = inverted ? (start + 1) : end;
		start = newOffset;
	}
	else
	{
		start = inverted ? (end - 1) : start;
		end = newOffset;

		// include the whole next unit if we even touch it
		if (m_roundingUnit && (end % m_roundingUnit == 0))
		{
			end += 1;
		}
	}

	// round to whole "units" if needed
	if (m_roundingUnit > 0)
	{
		end = Hax::Maths::RoundUp(end, m_roundingUnit);
		start = Hax::Maths::RoundDown(start, m_roundingUnit);
	}

	m_selection.SetRange(start, end);
}

