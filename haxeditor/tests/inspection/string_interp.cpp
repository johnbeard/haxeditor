/*
 * integer_interp.cpp
 *
 *  Created on: 1 Feb 2016
 *      Author: John Beard
 */

#include <gtest/gtest.h>

#include "haxeditor/inspection/DataInterpreter.h"

TEST(StringInterpret, basicString)
{
	StringInterpretation si("0");

	EXPECT_STREQ("0", si.GetAsString().c_str());
}
