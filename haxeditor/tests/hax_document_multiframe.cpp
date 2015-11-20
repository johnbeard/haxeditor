/*
 * hax_document_multiframe.cpp
 *
 *  Created on: 20 Nov 2015
 *      Author: John Beard
 */

#include <gtest/gtest.h>

#include "haxeditor/HaxDocumentMultiFrame.h"

class TestMultiFrame: public HaxDocumentMultiFrame
{
public:
	TestMultiFrame(HaxDocument& doc):
		HaxDocumentMultiFrame(doc)
	{
		setRowLength(80); //TODO drive externally?
	}

private:

	void onOffsetChangeInt() override
	{

	}

	void onSelectionChangedInt() override
	{

	}
};

class HMFTest: public ::testing::Test
{
public:

	HMFTest():
		mf(doc),
		sel(doc.GetSelection())
	{
	}

	HaxDocument doc;
	TestMultiFrame mf;
	const HaxDocument::Selection& sel; // handy shorthand
};

TEST_F(HMFTest, simpleMovement)
{
	// start at 0
	EXPECT_EQ(0, doc.GetOffset());

	// move one to the right
	mf.scrollRight(1, false);

	// moves one byte
	EXPECT_EQ(8, doc.GetOffset());

	// select one byte right
	mf.scrollRight(1, true);
	EXPECT_EQ(16, doc.GetOffset());
	EXPECT_EQ(8, sel.GetStart());
	EXPECT_EQ(16, sel.GetEnd());

	// and another
	mf.scrollRight(1, true);
	EXPECT_EQ(24, doc.GetOffset());
	EXPECT_EQ(8, sel.GetStart());
	EXPECT_EQ(24, sel.GetEnd());

	// and now go back
	mf.scrollRight(-2, true);
	EXPECT_EQ(8, doc.GetOffset());
	EXPECT_EQ(8, sel.GetStart());
	EXPECT_EQ(8, sel.GetEnd());

	// and now go back one more
	mf.scrollRight(-1, true);
	EXPECT_EQ(0, doc.GetOffset());
	EXPECT_EQ(0, sel.GetStart());
	EXPECT_EQ(8, sel.GetEnd());
}

TEST_F(HMFTest, selectionWithInversion)
{
	// start at 0
	EXPECT_EQ(0, doc.GetOffset());

	// move a bit to the right
	mf.scrollRight(5, false);

	// select one to the left
	mf.scrollRight(-1, true);

	EXPECT_EQ(32, doc.GetOffset()); // now at byte 5 start
	EXPECT_EQ(32, sel.GetStart());
	EXPECT_EQ(40, sel.GetEnd());

	// and back to start
	mf.scrollRight(1, true);

	EXPECT_EQ(40, doc.GetOffset()); // now at byte 5 start
	EXPECT_EQ(40, sel.GetStart());
	EXPECT_EQ(40, sel.GetEnd());

	// and now "invert" past the original selection start
	mf.scrollRight(1, true);

	EXPECT_EQ(48, doc.GetOffset()); // now at byte 5 start
	EXPECT_EQ(40, sel.GetStart());
	EXPECT_EQ(48, sel.GetEnd());

	// and keep going
	mf.scrollRight(1, true);

	EXPECT_EQ(56, doc.GetOffset()); // now at byte 5 start
	EXPECT_EQ(40, sel.GetStart());
	EXPECT_EQ(56, sel.GetEnd());
}
