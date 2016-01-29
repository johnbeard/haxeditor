/*
 * hax_string_renderer.cpp
 *
 *  Created on: 11 Nov 2015
 *      Author: John Beard
 */

#include <gtest/gtest.h>

#include "haxeditor/haxeditor.h"
#include "haxeditor/StringCellRenderer.h"

TEST(HaxStringRendererTests, basicOffsetCalcs)
{
	HaxDocument doc;
	HaxHexRenderer rdr(doc);

	rdr.SetWidth(80); // 10 bytes

	// sanity checking on the _Hex_ part before we go any further
	ASSERT_EQ(4u, rdr.GetBitsPerChar());
	ASSERT_EQ(2u, rdr.GetCellChars());
	ASSERT_EQ(10u, rdr.GetCellsPerRow());

	// can we work out the offset correctly?
	EXPECT_EQ(0u, rdr.GetRowForOffset(0, true));
	EXPECT_EQ(0u, rdr.GetRowForOffset(79, true));

	// can we tell the difference at a row end?
	EXPECT_EQ(0u, rdr.GetRowForOffset(80, false));
	EXPECT_EQ(1u, rdr.GetRowForOffset(80, true));

	EXPECT_EQ(10u, rdr.GetRowForOffset(800, true));

	DataCellRenderer::StringLinePos pos;
	pos = rdr.GetOffsetPosInLine(0, true);
	EXPECT_EQ(0u, pos.chars);
	EXPECT_EQ(0u, pos.gaps);

	pos = rdr.GetOffsetPosInLine(10, true); //bit 10 is cell 1, char 1
	EXPECT_EQ(2u, pos.chars);
	EXPECT_EQ(1u, pos.gaps);

	pos = rdr.GetOffsetPosInLine(15, true); // bit 15 is cell 1, char 2
	EXPECT_EQ(3u, pos.chars);
	EXPECT_EQ(1u, pos.gaps);

	pos = rdr.GetOffsetPosInLine(15, false); // left/right edge doesn't matter if within a cell
	EXPECT_EQ(3u, pos.chars);
	EXPECT_EQ(1u, pos.gaps);

	pos = rdr.GetOffsetPosInLine(16, true); // bit 16 left edge is very start of cell 2
	EXPECT_EQ(4u, pos.chars);
	EXPECT_EQ(2u, pos.gaps);

	pos = rdr.GetOffsetPosInLine(16, false); // bit 16 right edge is very end of cell 1
	EXPECT_EQ(4u, pos.chars);
	EXPECT_EQ(1u, pos.gaps);

	// new line
	pos = rdr.GetOffsetPosInLine(80, true);
	EXPECT_EQ(0u, pos.chars);
	EXPECT_EQ(0u, pos.gaps);
}

TEST(HaxStringRendererTests, lineStartEndPos)
{
	HaxDocument doc;
	HaxHexRenderer rdr(doc);

	rdr.SetWidth(80); // 10 bytes

	{
		const auto pos = rdr.GetLineStartPos();

		EXPECT_EQ(0u, pos.chars);
		EXPECT_EQ(0u, pos.gaps);
	}

	{
		const auto pos = rdr.GetLineEndPos();

		EXPECT_EQ(20u, pos.chars); // 10 bytes * 2 chars / byte
		EXPECT_EQ(9u, pos.gaps);
	}
}

TEST(HaxStringRendererTests, testZeroWidth)
{
	HaxDocument doc;
	HaxHexRenderer rdr(doc);

	rdr.SetWidth(0); // zero bytes

	// shouldn't divide by zero!
	DataCellRenderer::StringLinePos pos;
	pos = rdr.GetOffsetPosInLine(0, true);

	EXPECT_EQ(0u, pos.chars);
	EXPECT_EQ(0u, pos.gaps);
}

TEST(HaxStringRendererTests, hexCellList)
{
	HaxDocument doc;
	HaxHexRenderer rdr(doc);

	rdr.SetWidth(80);

	// shouldn't divide by zero!
	DataCellRenderer::StringLinePos pos;
	DataCellRenderer::CellList cells = rdr.GetRowCells(0);

	EXPECT_EQ(10u, cells.size());
	EXPECT_EQ(2, cells[0]);
}
