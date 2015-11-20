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
	 EXPECT_EQ(0, doc.GetOffset());


}



