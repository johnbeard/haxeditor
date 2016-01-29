/*
 * NullDataStream.cpp
 *
 *  Created on: 28 Jan 2016
 *      Author: John Beard
 */

#include "DataStream.h"

#include <boost/iostreams/device/null.hpp>

class NullDataStream::PImpl
{
public:
	boost::iostreams::null_source m_source;
};

NullDataStream::NullDataStream():
		m_impl(new PImpl)
{}

NullDataStream::~NullDataStream()
{}

void NullDataStream::ApplyToStream(FilteringIStream& stream)
{
	stream.push(m_impl->m_source);
}
