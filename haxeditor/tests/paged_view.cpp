/*
 * paged_view.cpp
 *
 *  Created on: 5 Nov 2015
 *      Author: John Beard
 *
 *  Tests for the PagedView class
 */

#include <gtest/gtest.h>

#include "haxeditor/PagedView.h"

class PagedViewTest: public ::testing::Test
{
public:
	PagedViewTest():
		scrolled(false)
	{
		pv.signal_pageStartChanged.connect(sigc::mem_fun(this,
				&PagedViewTest::onPageStartChange));
	}

	bool getAndResetScrolledFlag()
	{
		auto val = scrolled;
		scrolled = false;
		return val;
	}

	void onPageStartChange(const PagedView&)
	{
		scrolled = true;
	}

	PagedView pv;
	bool scrolled;
};

TEST_F(PagedViewTest, basicSetup)
{
	pv.SetPageSize(100);
	pv.SetRowLength(10);

	EXPECT_EQ(100, pv.GetPageSize());

	// scroll _down_ to show offset 200
	pv.ScrollToKeepOffsetInView(200);

	// we should have moved
	EXPECT_TRUE(getAndResetScrolledFlag());

	// and the starting row should be the start of row 11, such that row 20 is
	// showing
	EXPECT_EQ(110, pv.GetPageStart());

	// scrolling to the same place should do nothing
	pv.ScrollToKeepOffsetInView(200);

	// didn't move
	EXPECT_FALSE(getAndResetScrolledFlag());
	EXPECT_EQ(110, pv.GetPageStart());


	// scrolling up less than a page from the current position als doesn't
	// do anything
	pv.ScrollToKeepOffsetInView(150);

	// didn't move
	EXPECT_FALSE(getAndResetScrolledFlag());
	EXPECT_EQ(110, pv.GetPageStart());

	// scrolling up more than a page from the current position does the scroll
	pv.ScrollToKeepOffsetInView(65);

	// didn't move
	EXPECT_TRUE(getAndResetScrolledFlag());

	// page start is on a row boundary
	EXPECT_EQ(60, pv.GetPageStart());
}

TEST_F(PagedViewTest, maxOffsetNone)
{
	pv.SetPageSize(100);
	pv.SetRowLength(10);

	// no max offset, no limit
	pv.ScrollToKeepOffsetInView(1000000);

	EXPECT_EQ(1000000 - 90, pv.GetPageStart());
}

TEST_F(PagedViewTest, maxOffsetSet)
{
	pv.SetPageSize(100);
	pv.SetRowLength(10);
	pv.SetMaximumOffset(1000);

	// scroll within range is fine
	pv.ScrollToKeepOffsetInView(100);

	EXPECT_EQ(10, pv.GetPageStart());

	// end of range
	pv.ScrollToKeepOffsetInView(1000);

	EXPECT_TRUE(getAndResetScrolledFlag());
	EXPECT_EQ(900, pv.GetPageStart());

	pv.ScrollToKeepOffsetInView(2000);

	EXPECT_FALSE(getAndResetScrolledFlag());
	EXPECT_EQ(900, pv.GetPageStart());
}

TEST_F(PagedViewTest, maxOffsetVeryShort)
{
	pv.SetPageSize(100);
	pv.SetRowLength(10);
	pv.SetMaximumOffset(50);

	// within page, but within allowed range
	pv.ScrollToKeepOffsetInView(70);

	EXPECT_FALSE(getAndResetScrolledFlag());
	EXPECT_EQ(0, pv.GetPageStart());
}
