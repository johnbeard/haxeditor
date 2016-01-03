/*
 * mem_da.cpp
 *
 *  Created on: 22 Dec 2015
 *      Author: John Beard
 */

#include "dal/DataAbstractionLayer.h"

#include <gtest/gtest.h>

TEST(MemDA, basicSetup)
{
	MemoryDataAbstraction mda;

	// read only memory
	// TODO this will change...
	EXPECT_TRUE(mda.IsReadOnly());

	EXPECT_EQ(0u, mda.GetBaseDataLength());
}
