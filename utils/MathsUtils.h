/*
 * MathsUtils.h
 *
 *  Created on: 24 Dec 2015
 *      Author: John Beard
 */

#ifndef HAXEDITOR_UTILS_MATHSUTILS_H_
#define HAXEDITOR_UTILS_MATHSUTILS_H_

namespace Hax
{
namespace Maths
{
	/*!
	 * Rounds a number up to the next higher multiple (or the number itself
	 * if the multiple is 0 or the number is already a multiple)
	 */
	unsigned RoundUp(unsigned numToRound, unsigned multiple);

	/*!
	 * Rounds a number down to the next lower multiple
	 */
	unsigned RoundDown(unsigned numToRound, unsigned multiple);
}
}

#endif /* HAXEDITOR_UTILS_MATHSUTILS_H_ */
