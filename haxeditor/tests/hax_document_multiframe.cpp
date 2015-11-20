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
		mf(doc)
	{
	}

	HaxDocument doc;
	TestMultiFrame mf;
};

TEST_F(HMFTest, simpleMovement)
{
	const HaxDocument::Selection& sel = doc.GetSelection();

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

}



