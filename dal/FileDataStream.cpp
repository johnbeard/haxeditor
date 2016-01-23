/*
 * FileDataStream.cpp
 *
 *  Created on: 27 Jan 2016
 *      Author: John Beard
 */

#include "DataStream.h"

#include "utils/CompilerSupport.h"

#include <fstream>
#include <iostream>
#include <boost/iostreams/device/file_descriptor.hpp>

class FileDataStream::PImpl
{
public:
	PImpl(const std::string& filename):
		m_source(filename)
	{
		{
			// store the file size, since we can't get it from the stream easily
			std::ifstream is (filename, std::ifstream::binary);
			is.seekg (0, is.end);
			m_fileSize = is.tellg();
			is.seekg (0, is.beg);
		}
	}

	offset_t m_fileSize = 0;
	boost::iostreams::file_descriptor_source m_source;
};

// Public interface implementation wrapper

FileDataStream::FileDataStream(const std::string& filename):
	m_impl(std::make_unique<PImpl>(filename))
{
	//std::cout << "Setting up file stream: " << filename << std::endl;
}

FileDataStream::~FileDataStream()
{}

void FileDataStream::ApplyToStream(FilteringIStream& stream)
{
	stream.push(m_impl->m_source);
}

offset_t FileDataStream::GetDataLength() const
{
	return m_impl->m_fileSize;
}
