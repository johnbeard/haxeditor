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

	void onPageStartChangedInt() override
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
	EXPECT_EQ(0u, doc.GetOffset());

	// move one to the right
	mf.scrollRight(1, false);

	// moves one byte
	EXPECT_EQ(8u, doc.GetOffset());

	// select one byte right
	mf.scrollRight(1, true);
	EXPECT_EQ(16u, doc.GetOffset());
	EXPECT_EQ(8u, sel.GetStart());
	EXPECT_EQ(16u, sel.GetEnd());

	// and another
	mf.scrollRight(1, true);
	EXPECT_EQ(24u, doc.GetOffset());
	EXPECT_EQ(8u, sel.GetStart());
	EXPECT_EQ(24u, sel.GetEnd());

	// and now go back
	mf.scrollRight(-2, true);
	EXPECT_EQ(8u, doc.GetOffset());
	EXPECT_EQ(8u, sel.GetStart());
	EXPECT_EQ(8u, sel.GetEnd());

	// and now go back one more
	mf.scrollRight(-1, true);
	EXPECT_EQ(0u, doc.GetOffset());
	EXPECT_EQ(0u, sel.GetStart());
	EXPECT_EQ(8u, sel.GetEnd());
}

TEST_F(HMFTest, selectionWithInversion)
{
	// start at 0
	EXPECT_EQ(0u, doc.GetOffset());

	// move a bit to the right
	mf.scrollRight(5, false);

	// select one to the left
	mf.scrollRight(-1, true);

	EXPECT_EQ(32u, doc.GetOffset()); // now at byte 5 start
	EXPECT_EQ(32u, sel.GetStart());
	EXPECT_EQ(40u, sel.GetEnd());

	// and back to start
	mf.scrollRight(1, true);

	EXPECT_EQ(40u, doc.GetOffset()); // now at byte 5 start
	EXPECT_EQ(40u, sel.GetStart());
	EXPECT_EQ(40u, sel.GetEnd());

	// and now "invert" past the original selection start
	mf.scrollRight(1, true);

	EXPECT_EQ(48u, doc.GetOffset()); // now at byte 5 start
	EXPECT_EQ(40u, sel.GetStart());
	EXPECT_EQ(48u, sel.GetEnd());

	// and keep going
	mf.scrollRight(1, true);

	EXPECT_EQ(56u, doc.GetOffset()); // now at byte 5 start
	EXPECT_EQ(40u, sel.GetStart());
	EXPECT_EQ(56u, sel.GetEnd());
}

TEST_F(HMFTest, scrollTo)
{
	// start at 0
	EXPECT_EQ(0u, doc.GetOffset());

	mf.scrollTo(100, false, false);

	// check the document offset is updated
	EXPECT_EQ(100u, doc.GetOffset());

	mf.scrollTo(55, true, true); // need to move caret to do selection!

	// check selection extend works
	EXPECT_EQ(55u, doc.GetOffset());
	EXPECT_EQ(55u, sel.GetStart());
	EXPECT_EQ(100u, sel.GetEnd());
}

TEST_F(HMFTest, movePageStart)
{
	// start at 0
	EXPECT_EQ(0u, mf.GetPageOffset());

	mf.scrollPageStart(2);

	EXPECT_EQ(160u, mf.GetPageOffset());

	mf.scrollPageStart(-1);

	EXPECT_EQ(80u, mf.GetPageOffset());
}
