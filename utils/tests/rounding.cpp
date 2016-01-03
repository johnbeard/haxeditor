/*
 * selection_driver.cpp
 *
 *  Created on: 16 Nov 2015
 *      Author: John Beard
 */

#include <gtest/gtest.h>

#include "utils/MathsUtils.h"

using namespace Hax::Maths;

TEST(Rounding, roundUpToMultiple)
{
	// null case
	EXPECT_EQ(0u, RoundUp(0, 0));

	// num = multiple unchanged
	EXPECT_EQ(8u, RoundUp(8, 8));

	// round up
	EXPECT_EQ(16u, RoundUp(9, 8));
}

TEST(Rounding, roundDownToMultiple)
{
	// null case
	EXPECT_EQ(0u, RoundDown(0, 0));

	// num = multiple unchanged
	EXPECT_EQ(8u, RoundDown(8, 8));

	// round down to 0
	EXPECT_EQ(0u, RoundDown(7, 8));
	EXPECT_EQ(16u, RoundDown(20, 8));
}
