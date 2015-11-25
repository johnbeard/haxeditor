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
	SelectionDriverTest():
		sel(),
		driver(sel)
	{
		sel.signal_changed.connect(sigc::mem_fun(this, &SelectionDriverTest::offsetChanged));
	}

	void offsetChanged(const HaxDocument::Selection& /*changedSel*/)
	{
		m_callbackHit = true;
	}

	bool m_callbackHit = false;
	HaxDocument::Selection sel;
	SelectionDriver driver;
};

TEST_F(SelectionDriverTest, basicRightwardsFromZero)
{
	// init to false
	EXPECT_FALSE(driver.IsActive());

	// activate selection
	driver.SetActive(true);

	EXPECT_TRUE(driver.IsActive());

	// move the right edge 10 units
	driver.SetActiveType(SelectionDriver::Right);
	driver.onOffsetChanged(10);

	EXPECT_TRUE(m_callbackHit);
	m_callbackHit = false;

	EXPECT_EQ(0, sel.GetStart());
	EXPECT_EQ(10, sel.GetEnd());

	// and now shorten the offset
	driver.onOffsetChanged(5);

	EXPECT_TRUE(m_callbackHit);

	EXPECT_EQ(0, sel.GetStart());
	EXPECT_EQ(5, sel.GetEnd());

	// and now move the left edge
	driver.SetActiveType(SelectionDriver::Left);
	driver.onOffsetChanged(2);

	EXPECT_EQ(2, sel.GetStart());
	EXPECT_EQ(5, sel.GetEnd());

	// and move back
	driver.onOffsetChanged(1);

	EXPECT_EQ(1, sel.GetStart());
	EXPECT_EQ(5, sel.GetEnd());

	// and now "invert" by moving the start past the end
	driver.onOffsetChanged(12);

	EXPECT_EQ(5, sel.GetStart());
	EXPECT_EQ(12, sel.GetEnd());

	// and move the end past the start
	driver.SetActiveType(SelectionDriver::Right);
	driver.onOffsetChanged(4);

	EXPECT_EQ(4, sel.GetStart());
	EXPECT_EQ(5, sel.GetEnd());
}

TEST_F(SelectionDriverTest, activation)
{
	// init to false
	EXPECT_FALSE(driver.IsActive());

	// activate selection
	driver.SetActive(true);

	EXPECT_TRUE(driver.IsActive());

	// move the right edge 10 units
	driver.SetActiveType(SelectionDriver::Right);
	driver.onOffsetChanged(10);

	EXPECT_TRUE(m_callbackHit);
	m_callbackHit = false;

	EXPECT_EQ(0, sel.GetStart());
	EXPECT_EQ(10, sel.GetEnd());

	// activate selection
	driver.SetActive(false);

	EXPECT_FALSE(driver.IsActive());

	// we got an update
	EXPECT_TRUE(m_callbackHit);
	m_callbackHit = false;

	// reset to 0
	EXPECT_EQ(0, sel.GetStart());
	EXPECT_EQ(0, sel.GetEnd());

	driver.onOffsetChanged(100);

	// unchanged
	EXPECT_FALSE(m_callbackHit);
	EXPECT_EQ(0, sel.GetStart());
	EXPECT_EQ(0, sel.GetEnd());
}

TEST_F(SelectionDriverTest, setToNonZeroStart)
{
	driver.ResetOffset(16);
	driver.SetActive(true);

	driver.SetActiveType(SelectionDriver::Left);
	driver.onOffsetChanged(8);

	EXPECT_EQ(8, sel.GetStart());
	EXPECT_EQ(16, sel.GetEnd());
}
