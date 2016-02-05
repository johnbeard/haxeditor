/*
 * DataInspector.cpp
 *
 *  Created on: 2 Feb 2016
 *      Author: John Beard
 */


#include "DataInspector.h"

#include <algorithm>

#include <boost/multiprecision/cpp_int.hpp>

#include "utils/CompilerSupport.h"

FixedIntegerInspector::FixedIntegerInspector(unsigned size, bool sign, Endianness endian):
	m_intSize(size),
	m_signed(sign),
	m_endian(endian)
{
}

std::unique_ptr<DataInterpretation> FixedIntegerInspector::GetInterpretation(
		RawStream& stream) const
{
	using namespace boost::multiprecision;

	// raw vector of bytes
	std::vector<RawStream::char_type> data;

	// read the right about of data in in file order
	for (unsigned i = 0; i < m_intSize; ++i)
	{
		RawStream::char_type c;
		if (stream.get(c))
		{
			data.push_back(c);
		}
		else
		{
			// stream error - can't interpret!
			return nullptr;
		}
	}

	// This limits us to 8-byte integer interpretation
	// could be fixed in future with a arbitrary precision lib?
	cpp_int value;

	// loop for accumulating values into the storage value
	auto accumulateLoop = [&](const RawStream::char_type charValue)
	{
		value = (value * 0x100) + charValue;
	};

	bool isNegative = false;

	if( m_endian == Endianness::Little )
	{
		isNegative = *data.rbegin() & 0x80;
		std::for_each(data.rbegin(), data.rend(), accumulateLoop);
	}
	else
	{
		isNegative = *data.begin() & 0x80;
		std::for_each(data.begin(), data.end(), accumulateLoop);
	}

	// 2's complement negative - subtract the range from the unsigned value
	if (m_signed && isNegative)
	{
		auto range = pow(cpp_int(2), m_intSize * 8);
		value -= range;
	}

	// TODO: use an integer interpretation type, not string (?)
	std::unique_ptr<DataInterpretation> di(
			std::make_unique<StringInterpretation>(value.str()));
	return di;
}

