/*
 * change_applicator.cpp
 *
 *  Created on: 23 Jan 2016
 *      Author: John Beard
 */

#include "../ChangeApplicator.h"

#include <gtest/gtest.h>

#include "utils/CompilerSupport.h"
#include "dal/DataStream.h"

#include "boost/filesystem.hpp"

#include <fstream>
TEST(ChangeApplicatorTests, basicFileReadNoChange)
{
	ChangeApplicator ca;

	boost::filesystem::path path(DALTEST_SAMPLE_FILES);
	path /= "helloworld.txt";

	std::unique_ptr<BaseDataStream> fds = std::make_unique<FileDataStream>(path.string());

	ca.SetBaseData(fds);

	auto& is = ca.GetDataStream();

	std::string s(std::istreambuf_iterator<char>(is), {});

	EXPECT_EQ(12, ca.GetDataLength());
	EXPECT_STREQ("Hello World\n", s.c_str());
}

#if 0
TEST(ChangeApplicatorTests, inversionFilter)
{
	ChangeApplicator ca;

	InvertingFilter invFilter;

	ca.AddDataFilter(invFilter);

	std::unique_ptr<BaseDataStream> fds = std::make_unique<FileDataStream>("/tmp/test.tx");

	ca.SetBaseData(fds);

	auto& is = ca.GetDataStream();

	std::cout << static_cast<char>(is.get());
}
#endif
