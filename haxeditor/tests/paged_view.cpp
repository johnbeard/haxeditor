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

TEST(PagedViewTest, basicSetup)
{
	PagedView pv;

	pv.SetPageSize(100);
	pv.SetRowLength(10);

	EXPECT_EQ(100, pv.GetPageSize());

	// scroll _down_ to show offset 200
	bool scrolled = pv.ScrollToKeepOffsetInView(200);

	// we should have moved
	EXPECT_TRUE(scrolled);

	// and the starting row should be the start of row 11, such that row 20 is
	// showing
	EXPECT_EQ(110, pv.GetPageStart());

	// scrolling to the same place should do nothing
	scrolled = pv.ScrollToKeepOffsetInView(200);

	// didn't move
	EXPECT_FALSE(scrolled);
	EXPECT_EQ(110, pv.GetPageStart());


	// scrolling up less than a page from the current position als doesn't
	// do anything
	scrolled = pv.ScrollToKeepOffsetInView(150);

	// didn't move
	EXPECT_FALSE(scrolled);
	EXPECT_EQ(110, pv.GetPageStart());

	// scrolling up more than a page from the current position does the scroll
	scrolled = pv.ScrollToKeepOffsetInView(65);

	// didn't move
	EXPECT_TRUE(scrolled);

	// page start is on a row boundary
	EXPECT_EQ(60, pv.GetPageStart());
}
