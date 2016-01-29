/*
 * ChangeApplicator.h
 *
 *  Created on: 21 Jan 2016
 *      Author: John Beard
 */

#ifndef DAL_CHANGEAPPLICATOR_H_
#define DAL_CHANGEAPPLICATOR_H_

#include "ChangeStack.h"
#include "DataStream.h"
#include "DataTransform.h"

#include <istream>

/*!
 * Class that represents a "document", to which changes can be made.
 *
 * Changes are mode though this class which will internally arrange them
 * usefully and make the data available as a continuous stream, as if it
 * was a single buffer
 */
class ChangeApplicator
{
public:

	ChangeApplicator();
	~ChangeApplicator();

	/*!
	 * Make a modification to the document.
	 *
	 * A modification is a change of some of the data, but only of content,
	 * not of length.
	 *
	 * @param start the start of the modification
	 * @param data the new data from that offset
	 */
	void AddModification(offset_t start, std::vector<uint8_t> data);

	// void AddDeletion
	// void AddInsertion

	/*!
	 * Sets the base data (over which the changes are overlaid). This could
	 * be a file or some memory data, etc
	 * @param data
	 */
	void SetBaseData(std::unique_ptr<BaseDataStream>& data);

	/*!
	 * Add a data filter to the filter chain
	 * @param filter
	 */
	void AddDataFilter(DataStreamFilter& filter);

	/*!
	 * Gets the data iterator for a given offset. Iterating this will step
	 * through the original data and the changes, providing a seamless stream
	 * of the changes document
	 *
	 * TODO shared ptr? we might need to invalidate??
	 */
	std::istream& GetDataStream();

	/*!
	 * Get the current total length of the data stream (base data plus any
	 * change/filter modifications)
	 * @return the total length in _bits_
	 */
	offset_t GetDataLength() const;


private:
	class PImpl;
	std::unique_ptr<PImpl> m_impl;
};



#endif /* DAL_CHANGEAPPLICATOR_H_ */
