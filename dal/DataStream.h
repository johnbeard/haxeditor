/*
 * DataStream.h
 *
 *  Created on: 25 Jan 2016
 *      Author: John Beard
 */

#ifndef DAL_DATASTREAM_H_
#define DAL_DATASTREAM_H_

#include "DalTypes.h"

#include <boost/iostreams/filtering_stream.hpp>

class ChangeApplicator;

/*!
 * Class that represents the "base" data for a document, for example, a file
 * or a chunk of memory
 */
class BaseDataStream
{
public:

	typedef boost::iostreams::filtering_istream FilteringIStream;

	virtual ~BaseDataStream()
	{}

	/*!
	 * Implement to assign this base stream as the base source for an applicator
	 * @return the istream (which is owned internally to the data stream) that
	 * will provide the data
	 */
	virtual void ApplyToStream(FilteringIStream& stream) = 0;

	/*!
	 * Returns the length of the base data, e.g. the file size or the
	 * memory buffer size, in bytes
	 * @return length in bytes
	 */
	virtual offset_t GetDataLength() const = 0;
};

/*!
 * A null data stream contains no data, but can still have changes applied to it
 */
class NullDataStream: public BaseDataStream
{
public:
	NullDataStream();
	~NullDataStream();

	void ApplyToStream(FilteringIStream& stream) override;

	/*!
	 * @return the size of the null stream, which is always 0
	 */
	offset_t GetDataLength() const override
	{
		return 0;
	}

private:
	class PImpl;
	std::unique_ptr<PImpl> m_impl;
};

/*!
 * A null data stream that returns a preset pattern of data - useful for
 * testing and mocking.
 *
 * The pattern is just a simple counter
 */
class MockPatternDataStream: public BaseDataStream
{
public:
	MockPatternDataStream(offset_t length);
	~MockPatternDataStream();

	void ApplyToStream(FilteringIStream& stream) override;

	/*!
	 * @return the size of the null stream, which is always 0
	 */
	offset_t GetDataLength() const override;

private:
	class PImpl;
	std::unique_ptr<PImpl> m_impl;
};

#include <string>

class FileDataStream: public BaseDataStream
{
public:
	FileDataStream(const std::string& filename);
	~FileDataStream();

	void ApplyToStream(FilteringIStream& stream) override;

	/*!
	 * @return the size of the file
	 */
	offset_t GetDataLength() const override;

private:
	class PImpl;
	std::unique_ptr<PImpl> m_impl;
};




#endif /* DAL_DATASTREAM_H_ */
