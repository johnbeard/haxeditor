/*
 * selection_renderer.cpp
 *
 *  Created on: 12 Nov 2015
 *      Author: John Beard
 */


#include <gtest/gtest.h>

#include "haxeditor/SelectionRenderer.h"

class SelectionPathRendererTest: public ::testing::Test
{
public:
	SelectionPathRendererTest():
		hexR(doc),
		spr(hexR)
	{
		hexR.SetWidth(80);

		SelectionRenderer::Layout layout;

		layout.charH = 12;
		layout.charW = 7;
		layout.interCellGap = 6;
		layout.interRowGap = 8;

		spr.SetLayout(layout);
	}

	HaxDocument doc;
	HaxHexRenderer hexR;
	SelectionPathRenderer spr;
	HaxDocument::Selection selection;

protected:
	void checkSaneRectangle(const SelectionPathRenderer::PointList& rectRegion) const
	{
		EXPECT_EQ(rectRegion[0].x, rectRegion[3].x);
		EXPECT_EQ(rectRegion[1].x, rectRegion[2].x);

		EXPECT_EQ(rectRegion[0].y, rectRegion[1].y);
		EXPECT_EQ(rectRegion[2].y, rectRegion[3].y);
	}
};

TEST_F(SelectionPathRendererTest, pathNullSelection)
{
	selection.SetRange(8, 8); // select nothing
	spr.UpdateSelection(selection);

	const auto& regions = spr.GetPaths();

	EXPECT_EQ(0u, regions.size());
}

TEST_F(SelectionPathRendererTest, pathOneLineStart)
{
	selection.SetRange(0, 16); // select the first two bytes
	spr.UpdateSelection(selection);

	const auto& regions = spr.GetPaths();

	EXPECT_EQ(1u, regions.size());
	EXPECT_EQ(4u, regions[0].size());

	EXPECT_EQ(0, regions[0][0].x);
	EXPECT_EQ(14 * 2 + 6, regions[0][1].x);

	EXPECT_EQ(0, regions[0][0].y);
	EXPECT_EQ(12, regions[0][2].y);

	checkSaneRectangle(regions[0]);
}

TEST_F(SelectionPathRendererTest, pathOneLineMiddle)
{
	selection.SetRange(8, 16); // select the second byte only
	spr.UpdateSelection(selection);

	const auto& regions = spr.GetPaths();

	EXPECT_EQ(1u, regions.size());
	EXPECT_EQ(4u, regions[0].size());

	EXPECT_EQ(14 + 6, regions[0][0].x);
	EXPECT_EQ(14 * 2 + 6, regions[0][1].x);

	EXPECT_EQ(0, regions[0][0].y);
	EXPECT_EQ(12, regions[0][2].y);

	checkSaneRectangle(regions[0]);
}

TEST_F(SelectionPathRendererTest, pathOneLineEnd)
{
	selection.SetRange(72, 80); // select the end
	spr.UpdateSelection(selection);

	const auto& regions = spr.GetPaths();

	EXPECT_EQ(1u, regions.size());
	EXPECT_EQ(4u, regions[0].size());

	EXPECT_EQ(14 * 9 + 6 * 9, regions[0][0].x);
	EXPECT_EQ(14 * 10 + 6 * 9, regions[0][1].x);

	EXPECT_EQ(0, regions[0][0].y);
	EXPECT_EQ(12, regions[0][2].y);

	checkSaneRectangle(regions[0]);
}

TEST_F(SelectionPathRendererTest, pathTwoLines)
{
	selection.SetRange(72, 88); // select the last byte on line 0 and first on line 1
	spr.UpdateSelection(selection);

	const auto& regions = spr.GetPaths();

	// 1 joined-up region
	EXPECT_EQ(1u, regions.size());
	EXPECT_EQ(8u, regions[0].size());

	EXPECT_EQ(9 * (2 * 7) + 9 * 6, regions[0][0].x);
	//EXPECT_EQ(10 * (2 * 7) + 10 * 6, regions[0][1].x);
}

TEST_F(SelectionPathRendererTest, rowPositionsForOffset)
{
	// check origin
	EXPECT_EQ(0u, spr.GetOffsetForPosition(0, 0));

	// check that the row to row offset works
	EXPECT_EQ(0u, spr.GetOffsetForPosition(0, 12));
	EXPECT_EQ(0u, spr.GetOffsetForPosition(0, 12 + 8 - 1));
	EXPECT_EQ(80u, spr.GetOffsetForPosition(0, 12 + 8));

	// anywhere in the first cell or first gap is 0
	EXPECT_EQ(0u, spr.GetOffsetForPosition(14, 0));
	EXPECT_EQ(0u, spr.GetOffsetForPosition(14 + 6 - 1, 0));

	// and the second cell starts _after_ the first gap
	EXPECT_EQ(8u, spr.GetOffsetForPosition(14 + 6, 0));
}
