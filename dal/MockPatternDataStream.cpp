/*
 * MemoryDataStream.cpp
 *
 *  Created on: 28 Jan 2016
 *      Author: John Beard
 */

#include "DataStream.h"

#include <cassert>
#include <iostream>

#include <boost/iostreams/device/null.hpp>

struct mock_pattern_source_tag: boost::iostreams::device_tag,
								boost::iostreams::input_seekable
{};


struct IncCounterSource
{
public:
	typedef mock_pattern_source_tag category;
	typedef char char_type;

	IncCounterSource(offset_t length):
		m_length(length)
	{
	}

	std::streamsize read(char* s, std::streamsize n)
	{
		//std::cout << "Inc src read " << n << " from " << m_pos << std::endl;
		// we have already overstepped the buffer
		if (m_pos >= m_length)
			return -1;

		const auto oldPos = m_pos;
		const auto writeUpTo = std::min(m_pos + n, m_length);

		for (int i = 0; m_pos < writeUpTo; ++m_pos, ++i)
		{
			s[i] = m_pos & 0xFF;
		}

		assert(m_pos == writeUpTo);
		return m_pos - oldPos;
	}

	std::streampos seek(boost::iostreams::stream_offset off, std::ios_base::seekdir way)
	{
		std::streampos newPos;

		if (way == std::ios_base::beg)
		{
			newPos = off;
		}
		else if (way == std::ios_base::cur)
		{
			newPos = m_pos + static_cast<std::streamoff>(off);
		}
		else if (way == std::ios_base::end)
		{
			newPos = m_length - static_cast<std::streamoff>(off);
		}

		// clamp to acceptable range
		if (newPos < 0)
			newPos = 0;
		else if (newPos > m_length)
			newPos = m_length;

		m_pos = newPos;
		return newPos;
	}

	std::streamsize GetSize() const
	{
		return m_length;
	}

private:
	offset_t m_pos = 0;
	offset_t m_length = 0;
};

class MockPatternDataStream::PImpl
{
public:
	PImpl(offset_t length):
		m_source(length)
	{}

	IncCounterSource m_source;
};

MockPatternDataStream::MockPatternDataStream(offset_t length):
		m_impl(new PImpl(length))
{}

MockPatternDataStream::~MockPatternDataStream()
{}

void MockPatternDataStream::ApplyToStream(FilteringIStream& stream)
{
	stream.push(m_impl->m_source);
}

offset_t MockPatternDataStream::GetDataLength() const
{
	return m_impl->m_source.GetSize();
}


