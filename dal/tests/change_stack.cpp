/*
 * change_stack.cpp
 *
 *  Created on: 20 Jan 2016
 *      Author: John Beard
 */

#include "dal/ChangeStack.h"

#include <gtest/gtest.h>


class ChangeStackTest: public ::testing::Test
{
public:

	std::unique_ptr<DataChange> constructModification(datasize_t size)
	{
		std::unique_ptr<DataChange> c(new ModifyChange(size));

		// TODO fill with obvious pattern

		return c;
	}

	const ChangeStack::ChList& list() const
	{
		return cs.GetChangeList();
	}

	ChangeStack cs;
};

TEST_F(ChangeStackTest, null)
{
	// no changes in the list
	EXPECT_EQ(0, cs.GetChangeList().size());
}

TEST_F(ChangeStackTest, singleChange)
{
	cs.AddChange(constructModification(5), 10);

	EXPECT_EQ(1, list().size());

	const auto chIt = list().begin();

	// expect just the inserted change
	EXPECT_EQ(10, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(5, (*chIt)->end);
}

TEST_F(ChangeStackTest, disjointChange)
{
	cs.AddChange(constructModification(5), 10);
	cs.AddChange(constructModification(6), 20);

	EXPECT_EQ(2, list().size());

	auto chIt = list().begin();

	// both changes should be as they were inserted
	EXPECT_EQ(10, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(5, (*chIt)->end);

	chIt++;

	EXPECT_EQ(20, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(6, (*chIt)->end);
}

TEST_F(ChangeStackTest, disjointChangeReverse)
{
	cs.AddChange(constructModification(6), 20);
	cs.AddChange(constructModification(5), 10);

	// ...222....111....

	EXPECT_EQ(2, list().size());

	auto chIt = list().begin();

	// both changes should be as they were inserted
	// but the other way around
	EXPECT_EQ(10, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(5, (*chIt)->end);

	chIt++;

	EXPECT_EQ(20, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(6, (*chIt)->end);
}

TEST_F(ChangeStackTest, abuttingChange)
{
	cs.AddChange(constructModification(5), 10);
	cs.AddChange(constructModification(6), 15);

	EXPECT_EQ(2, list().size());

	auto chIt = list().begin();

	// both changes should be as they were inserted
	EXPECT_EQ(10, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(5, (*chIt)->end);

	chIt++;

	EXPECT_EQ(15, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(6, (*chIt)->end);
}

TEST_F(ChangeStackTest, secondOverlapsFirstEnd)
{
	cs.AddChange(constructModification(5), 8);
	cs.AddChange(constructModification(6), 10);

	// ....111111...
	// ......2222222...

	EXPECT_EQ(2, list().size());

	auto chIt = list().begin();

	// first change is just the start of the first one
	EXPECT_EQ(8, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(2, (*chIt)->end);

	chIt++;

	// followed by the complete later change
	EXPECT_EQ(10, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(6, (*chIt)->end);
}

TEST_F(ChangeStackTest, secondOverlapsFirstExactly)
{
	cs.AddChange(constructModification(5), 10);
	cs.AddChange(constructModification(5), 10);

	// ...11111...
	// ...22222...

	ASSERT_EQ(1, list().size());

	auto chIt = list().begin();

	// the later change took precendence and replaces the first one exacty
	EXPECT_EQ(10, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(5, (*chIt)->end);
}

TEST_F(ChangeStackTest, secondOverlapsFirstWithDoubleOverhang)
{
	cs.AddChange(constructModification(5), 10);
	cs.AddChange(constructModification(12), 8);

	// .....1111....
	// ...2222222...

	EXPECT_EQ(1, list().size());

	auto chIt = list().begin();

	// only the later change is there and it is pristine
	EXPECT_EQ(8, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(12, (*chIt)->end);
}

TEST_F(ChangeStackTest, secondOverlapsFirstStart)
{
	cs.AddChange(constructModification(5), 10);
	cs.AddChange(constructModification(4), 8);

	EXPECT_EQ(2, list().size());

	auto chIt = list().begin();

	// later change is pristine
	EXPECT_EQ(8, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(4, (*chIt)->end);

	chIt++;

	// older change has the front missing
	EXPECT_EQ(10, (*chIt)->changeOffset);
	EXPECT_EQ(2, (*chIt)->start);
	EXPECT_EQ(5, (*chIt)->end);
}

TEST_F(ChangeStackTest, changeOverlapsMultipleCompletely)
{
	cs.AddChange(constructModification(5), 10);
	cs.AddChange(constructModification(6), 20);
	cs.AddChange(constructModification(50), 5);

	// ...11...22...
	// .3333333333..

	ASSERT_EQ(1, list().size());

	auto chIt = list().begin();

	// later change is pristine
	EXPECT_EQ(5, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(50, (*chIt)->end);
}

TEST_F(ChangeStackTest, changeOverlapsOneCompletelyAndOnePartly)
{
	cs.AddChange(constructModification(5), 10); // 10 - 15
	cs.AddChange(constructModification(6), 20); // 20 - 26
	cs.AddChange(constructModification(18), 6); //  6 - 24

	// ...11...2222..
	// .333333333....

	ASSERT_EQ(2, list().size());

	auto chIt = list().begin();

	// later change is pristine
	EXPECT_EQ(6, (*chIt)->changeOffset);
	EXPECT_EQ(0, (*chIt)->start);
	EXPECT_EQ(18, (*chIt)->end);

	chIt++;

	// first change is gone, second is truncated
	EXPECT_EQ(20, (*chIt)->changeOffset);
	EXPECT_EQ(4, (*chIt)->start);
	EXPECT_EQ(6, (*chIt)->end);
}
