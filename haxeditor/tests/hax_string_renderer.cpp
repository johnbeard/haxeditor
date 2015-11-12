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
	EXPECT_EQ(0, rdr.GetRowForOffset(0));
	EXPECT_EQ(0, rdr.GetRowForOffset(79));
	EXPECT_EQ(1, rdr.GetRowForOffset(80));
	EXPECT_EQ(10, rdr.GetRowForOffset(800));

	unsigned cells, chars;
	cells = rdr.GetColForOffset(0, &chars);
	EXPECT_EQ(0, cells);
	EXPECT_EQ(0, chars);

	cells = rdr.GetColForOffset(10, &chars); //bit 10 is cell 1, char 1
	EXPECT_EQ(1, cells);
	EXPECT_EQ(0, chars);

	cells = rdr.GetColForOffset(15, &chars);
	EXPECT_EQ(1, cells);
	EXPECT_EQ(1, chars);

	// new line
	cells = rdr.GetColForOffset(80, &chars);
	EXPECT_EQ(0, cells);
	EXPECT_EQ(0, chars);
}

TEST(HaxStringRendererTests, testZeroWidth)
{
	HaxDocument doc;
	HaxHexRenderer rdr(doc);

	rdr.SetWidth(0); // 10 bytes

	// shouldn't divide by zero!
	EXPECT_EQ(0, rdr.GetColForOffset(0, nullptr));
	EXPECT_EQ(0, rdr.GetRowForOffset(0));
}
