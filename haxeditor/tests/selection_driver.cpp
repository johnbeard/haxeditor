/*
 * selection_driver.cpp
 *
 *  Created on: 16 Nov 2015
 *      Author: John Beard
 */

#include <gtest/gtest.h>

#include "haxeditor/SelectionDriver.h"

class SelectionDriverTest: public ::testing::Test
{
public:
	SelectionDriverTest()
	{
		sel.signal_SelectionChanged.connect(sigc::mem_fun(this, &SelectionDriverTest::offsetChanged));
	}

	void offsetChanged(const HaxDocument::Selection& /*changedSel*/)
	{
		m_callbackHit = true;
	}

	bool m_callbackHit = false;
	HaxDocument::Selection sel;
};

TEST_F(SelectionDriverTest, basicRightwardsFromZero)
{
	SelectionDriver driver(sel);

	// init to false
	EXPECT_FALSE(driver.IsActive());

	// activate selection
	driver.SetActive(true);

	EXPECT_TRUE(driver.IsActive());

	// move the right edge 10 units
	driver.SetActiveType(SelectionDriver::Right);
	driver.onOffsetChanged(10);

	EXPECT_TRUE(m_callbackHit);

	EXPECT_EQ(0, sel.GetStart());
	EXPECT_EQ(10, sel.GetEnd());
}



