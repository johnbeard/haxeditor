/*
 * DataInterpreter.h
 *
 *  Created on: 1 Feb 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_INSPECTION_DATAINTERPRETER_H_
#define HAXEDITOR_INSPECTION_DATAINTERPRETER_H_

#include <string>

/*!
 * A data interpretation is the result of processing of some data though
 * some sort of inspector. For example, the raw data 0x01 could be inspected as
 * a char (giving 0x01) and the interpretation is then a simple string "1".
 * (note, integers actually probably will have a separate interpreter type, this
 * is just an example).
 *
 * The interpretation can then be show to the user in some sensible format
 * (for example strings can have encoding applied, ints can have separators
 * added, etc)
 */
class DataInterpretation
{
public:
	virtual ~DataInterpretation()
	{}

	/*!
	 * Formats the interpretation as a basic string.
	 *
	 * Do all interpretations support this? seems like it might be valid,
	 * even if it's only for cut-down summaries of very complex types?
	 * @return the string
	 */
	virtual std::string GetAsString() const = 0;
};

/*!
 * A StringInterpretation is a simple interpretation that represents the
 * stored interpretation a string.
 */
class StringInterpretation: public DataInterpretation
{
public:
	StringInterpretation(const std::string& data):
		m_dataString(data)
	{}

	std::string GetAsString() const override
	{
		return m_dataString;
	}

private:
	std::string m_dataString;
};


#endif /* HAXEDITOR_INSPECTION_DATAINTERPRETER_H_ */
