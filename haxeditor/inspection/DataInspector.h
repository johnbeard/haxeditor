/*
 * DataInspector.h
 *
 *  Created on: 1 Feb 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_INSPECTION_DATAINSPECTOR_H_
#define HAXEDITOR_INSPECTION_DATAINSPECTOR_H_

#include <haxeditor/inspection/DataInterpreter.h>

#include <memory>

typedef std::basic_istream<uint8_t> RawStream;

/*!
 * A DataInspector is a class that represents a single "inspection"
 * or interpretation of a chunk of data, for example, as an integer of certain
 * length and signedness
 */
class DataInspector
{
public:
	virtual ~DataInspector()
	{}

	/*!
	 * Inspect a stream of data.
	 *
	 * A new interpretation of the data found in the stream will be returned
	 * (which might indicate an error, if the stream is too short or otherwise
	 * defective in terms of this inspector)s
	 *
	 * @param stream the stream to inspect
	 * @return a new interpretation of the data
	 */
	virtual std::unique_ptr<DataInterpretation> GetInterpretation(
			RawStream& stream) const = 0;
};

/*!
 * Class to inspect a fixed length integer (eg char, int, uint64_t)
 */
class FixedIntegerInspector: public DataInspector
{
public:
	enum class Endianness {
		Little,
		Big
	};

	FixedIntegerInspector(unsigned size, bool sign, Endianness endian);

	std::unique_ptr<DataInterpretation> GetInterpretation(
			RawStream& stream) const override;

private:
	unsigned m_intSize;
	bool m_signed;
	Endianness m_endian;
};

#endif /* HAXEDITOR_INSPECTION_DATAINSPECTOR_H_ */
