/*
 * selection_renderer.cpp
 *
 *  Created on: 12 Nov 2015
 *      Author: john
 */


#include <gtest/gtest.h>

#include "haxeditor/SelectionRenderer.h"

static void checkSaneRectangle(const SelectionPathRenderer::PointList& rectRegion)
{
	EXPECT_EQ(rectRegion[0].x, rectRegion[3].x);
	EXPECT_EQ(rectRegion[1].x, rectRegion[2].x);

	EXPECT_EQ(rectRegion[0].y, rectRegion[1].y);
	EXPECT_EQ(rectRegion[2].y, rectRegion[3].y);
}

TEST(SelectionRenderer, pathOneLine)
{
	HaxDocument doc;
	HaxHexRenderer hexR(doc);
	SelectionPathRenderer spr(hexR);
	hexR.SetWidth(80);

	SelectionRenderer::Layout layout;
	layout.charH = 12;
	layout.charW = 7;
	layout.interCellGap = 6;
	layout.interRowGap = 8;

	spr.SetLayout(layout);

	HaxDocument::Selection selection;

	{
		selection.SetRange(0, 16); // select the first two bytes
		spr.UpdateSelection(selection);

		const auto& regions = spr.GetPaths();

		EXPECT_EQ(1, regions.size());
		EXPECT_EQ(4, regions[0].size());

		EXPECT_EQ(0, regions[0][0].x);
		EXPECT_EQ(14 * 2 + 6, regions[0][1].x);

		EXPECT_EQ(0, regions[0][0].y);
		EXPECT_EQ(12, regions[0][2].y);

		checkSaneRectangle(regions[0]);
	}

	{
		selection.SetRange(8, 16); // select the first two bytes
		spr.UpdateSelection(selection);

		const auto& regions = spr.GetPaths();

		EXPECT_EQ(1, regions.size());
		EXPECT_EQ(4, regions[0].size());

		EXPECT_EQ(14 + 6, regions[0][0].x);
		EXPECT_EQ(14 * 2 + 6, regions[0][1].x);

		EXPECT_EQ(0, regions[0][0].y);
		EXPECT_EQ(12, regions[0][2].y);

		checkSaneRectangle(regions[0]);
	}


}

