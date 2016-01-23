/*
 * ChangeApplicator.cpp
 *
 *  Created on: 22 Jan 2016
 *      Author: John Beard
 */

#include "ChangeApplicator.h"

#include "utils/CompilerSupport.h"

// boost includes for the stream filtering
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>


// ====
// Change stream filter
//

class ChangeFilterStream //: public StreamFilter?
{

};

// Change applicator..

class ChangeApplicator::PImpl
{
public:
	PImpl():
		m_stream(new boost::iostreams::filtering_istream())
	{

	}

	offset_t GetDataLength() const
	{
		return m_baseData->GetDataLength() + m_stack.GetLengthModification();
	}

	// The list of changes made by the user
	ChangeStack m_stack;

	// The base data of the change applicator
	std::unique_ptr<BaseDataStream> m_baseData;

	// THe set of filter stages applied to the base data
	// items are stored in "Logical" order - the first applied is the
	// first in the list
	std::vector<DataStreamFilter*> m_filters;

	std::unique_ptr<boost::iostreams::filtering_istream> m_stream;
};


ChangeApplicator::ChangeApplicator():
		m_impl(new PImpl)
{}


ChangeApplicator::~ChangeApplicator()
{}

void ChangeApplicator::AddModification(offset_t start, std::vector<uint8_t> data)
{
	// construct the DatAchange
	auto mod = std::make_unique<ModifyChange>(data.size());

	// add to the change stack at the right position
	m_impl->m_stack.AddChange(std::move(mod), start);
}

void ChangeApplicator::SetBaseData(std::unique_ptr<BaseDataStream>& data)
{
	// exchange the old source if there is one
	if (m_impl->m_stream->size())
		m_impl->m_stream->pop();

	m_impl->m_baseData = std::move(data);

	m_impl->m_baseData->ApplyToStream(*m_impl->m_stream);
}

void ChangeApplicator::AddDataFilter(DataStreamFilter& filter)
{
	m_impl->m_filters.push_back(&filter);

	// TODO deal with thisand add to the stack
}

std::istream& ChangeApplicator::GetDataStream()
{
    return *m_impl->m_stream;
}

offset_t ChangeApplicator::GetDataLength() const
{
	// defer to the impl, easier there
	return m_impl->GetDataLength();
}

