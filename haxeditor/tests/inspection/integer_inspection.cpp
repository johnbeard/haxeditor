/*
 * inteer_inspection.cpp
 *
 *  Created on: 1 Feb 2016
 *      Author: John Beard
 */

#include <gtest/gtest.h>

#include "haxeditor/inspection/DataInspector.h"

class VectorCharBuff : public std::basic_streambuf<uint8_t, std::char_traits<uint8_t> >
{
public:
	VectorCharBuff(std::initializer_list<uint8_t> values):
		vec(values)
	{
		setg(vec.data(), vec.data(), vec.data() + vec.size());
	}

private:
	std::vector<uint8_t> vec;
};

TEST(IntegerInspection, basicCharZEro)
{
	VectorCharBuff buff({0x00});
	RawStream is(&buff);

	FixedIntegerInspector ii(1, false, FixedIntegerInspector::Endianness::Little);

	auto interp = ii.GetInterpretation(is);

	EXPECT_STREQ("0", interp->GetAsString().c_str());
}

TEST(IntegerInspection, basicUInt32AllBytes)
{
	VectorCharBuff buff({0x81, 0x02, 0xFF, 0x83});
	RawStream is(&buff);

	FixedIntegerInspector ii(4, false, FixedIntegerInspector::Endianness::Little);

	auto interp = ii.GetInterpretation(is);

	EXPECT_STREQ("2214527617", interp->GetAsString().c_str());
}

TEST(IntegerInspection, basicLeSInt32AllBytes)
{
	VectorCharBuff buff({0x81, 0x02, 0xFF, 0x83});
	RawStream is(&buff);

	FixedIntegerInspector ii(4, true, FixedIntegerInspector::Endianness::Little);

	auto interp = ii.GetInterpretation(is);

	EXPECT_STREQ("-2080439679", interp->GetAsString().c_str());
}

TEST(IntegerInspection, basicBeSInt32AllBytes)
{
	VectorCharBuff buff({0x81, 0x02, 0xFF, 0x83});
	RawStream is(&buff);

	FixedIntegerInspector ii(4, true, FixedIntegerInspector::Endianness::Big);

	auto interp = ii.GetInterpretation(is);

	EXPECT_STREQ("-2130509949", interp->GetAsString().c_str());
}
