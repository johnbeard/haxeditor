/*
 * hax_string_renderer.cpp
 *
 *  Created on: 11 Nov 2015
 *      Author: John Beard
 */

#include <gtest/gtest.h>

#include "haxeditor/HaxOffsetRenderer.h"

TEST(HaxOffsetRendererTests, basicDecimal)
{
	HaxOffsetRendererDecimal hdr;

	// init to 0
	EXPECT_STREQ("0", hdr.RenderOffset().c_str());

	// set and try again
	hdr.SetOffset(13);
	EXPECT_STREQ("13", hdr.RenderOffset().c_str());
}

TEST(HaxRangeRendererTests, basic)
{
	HaxOffsetRendererDecimal hdr;
	HaxRangeRenderer rr;

	rr.SetRenderer(&hdr);

	EXPECT_STREQ("No selection", rr.RenderRange().c_str());

	rr.SetRange(10, 23);

	// remember, the range is really [10:23), not [10:23]
	EXPECT_STREQ("10 : 22", rr.RenderRange().c_str());
	// but that's still 13 units!
	EXPECT_STREQ("13", rr.RenderRangeSize().c_str());
}
