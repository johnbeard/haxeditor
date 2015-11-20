/*
 * hax_string_renderer.cpp
 *
 *  Created on: 11 Nov 2015
 *      Author: John Beard
 */

#include <gtest/gtest.h>

#include "haxeditor/haxeditor.h"

TEST(HaxStringRendererTests, basicOffsetCalcs)
{
	HaxDocument doc;
	HaxHexRenderer rdr(doc);

	rdr.SetWidth(80); // 10 bytes

	// sanity checking on the _Hex_ part before we go any further
	ASSERT_EQ(4, rdr.GetBitsPerChar());
	ASSERT_EQ(2, rdr.GetCellChars());
	ASSERT_EQ(10, rdr.GetCellsPerRow());

	// can we work out the offset correctly?
	EXPECT_EQ(0, rdr.GetRowForOffset(0, true));
	EXPECT_EQ(0, rdr.GetRowForOffset(79, true));

	// can we tell the difference at a row end?
	EXPECT_EQ(0, rdr.GetRowForOffset(80, false));
	EXPECT_EQ(1, rdr.GetRowForOffset(80, true));

	EXPECT_EQ(10, rdr.GetRowForOffset(800, true));

	HaxStringRenderer::StringLinePos pos;
	pos = rdr.GetOffsetPosInLine(0, true);
	EXPECT_EQ(0, pos.chars);
	EXPECT_EQ(0, pos.gaps);

	pos = rdr.GetOffsetPosInLine(10, true); //bit 10 is cell 1, char 1
	EXPECT_EQ(2, pos.chars);
	EXPECT_EQ(1, pos.gaps);

	pos = rdr.GetOffsetPosInLine(15, true); // bit 15 is cell 1, char 2
	EXPECT_EQ(3, pos.chars);
	EXPECT_EQ(1, pos.gaps);

	pos = rdr.GetOffsetPosInLine(15, false); // left/right edge doesn't matter if within a cell
	EXPECT_EQ(3, pos.chars);
	EXPECT_EQ(1, pos.gaps);

	pos = rdr.GetOffsetPosInLine(16, true); // bit 16 left edge is very start of cell 2
	EXPECT_EQ(4, pos.chars);
	EXPECT_EQ(2, pos.gaps);

	pos = rdr.GetOffsetPosInLine(16, false); // bit 16 right edge is very end of cell 1
	EXPECT_EQ(4, pos.chars);
	EXPECT_EQ(1, pos.gaps);

	// new line
	pos = rdr.GetOffsetPosInLine(80, true);
	EXPECT_EQ(0, pos.chars);
	EXPECT_EQ(0, pos.gaps);
}

TEST(HaxStringRendererTests, lineStartEndPos)
{
	HaxDocument doc;
	HaxHexRenderer rdr(doc);

	rdr.SetWidth(80); // 10 bytes

	{
		const auto pos = rdr.GetLineStartPos();

		EXPECT_EQ(0, pos.chars);
		EXPECT_EQ(0, pos.gaps);
	}

	{
		const auto pos = rdr.GetLineEndPos();

		EXPECT_EQ(20, pos.chars); // 10 bytes * 2 chars / byte
		EXPECT_EQ(9, pos.gaps);
	}
}

TEST(HaxStringRendererTests, testZeroWidth)
{
	HaxDocument doc;
	HaxHexRenderer rdr(doc);

	rdr.SetWidth(0); // 10 bytes

	// shouldn't divide by zero!
	HaxStringRenderer::StringLinePos pos;
	pos = rdr.GetOffsetPosInLine(0, true);

	EXPECT_EQ(0, pos.chars);
	EXPECT_EQ(0, pos.gaps);
}
