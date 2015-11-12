/*
 * selection_renderer.cpp
 *
 *  Created on: 12 Nov 2015
 *      Author: john
 */


#include <gtest/gtest.h>

#include "haxeditor/SelectionRenderer.h"

TEST(SelectionRenderer, pathOneLine)
{
	HaxDocument doc;
	HaxHexRenderer hexR(doc);
	SelectionPathRenderer spr(hexR);

	SelectionRenderer::Layout layout;
	layout.charH = 12;
	layout.charW = 7;
	layout.interCellGap = 6;
	layout.interRowGap = 8;

	spr.SetLayout(layout);

	HaxDocument::Selection selection;
	selection.SetRange(8, 16); // select the second byte

	spr.UpdateSelection(selection);

	const auto& regions = spr.GetPaths();

	EXPECT_EQ(1, regions.size());
	EXPECT_EQ(4, regions[0].size());
}

