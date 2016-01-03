/*
 * MathsUtils.cpp
 *
 *  Created on: 24 Dec 2015
 *      Author: John Beard
 */

#include "MathsUtils.h"

/*!
 * Rounds a number up to the next higher multiple
 */
unsigned Hax::Maths::RoundUp(unsigned numToRound, unsigned multiple)
{
	if (multiple == 0)
		return numToRound;

	int remainder = numToRound % multiple;
	if (remainder == 0)
		return numToRound;

	return numToRound + multiple - remainder;
}

/*!
 * Rounds a number down to the next lower multiple
 */
unsigned Hax::Maths::RoundDown(unsigned numToRound, unsigned multiple)
{
	if (multiple == 0)
		return numToRound;

	return (numToRound / multiple) * multiple;
}
