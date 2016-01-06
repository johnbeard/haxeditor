/*
 * DataChange.h
 *
 *  Created on: 6 Jan 2016
 *      Author: John Beard
 */

#ifndef DAL_DATACHANGE_H_
#define DAL_DATACHANGE_H_

/*!
 * Modification to some data - could be insertion, deletion, modification
 * (are there more?)
 *
 * These changes are exclusively binary data modifications, and are the
 * low-level representation of modifications made at the application level, e.g.
 *   - changing a byte to a new value (simple translation to a DataChange)
 *   - inverting a byte - also converted to a DataChange, but the value is\n
 *     not directly set by the user
 *   - more complete data transforms might be implemented as a sequence of\n
 *     DataChanges (and the application might roll them back as a single unit)
 *
 * It is up the the application to manage these changes as it sees fit.
 */
class DataChange
{
public:
	// these are managed via the base pointers
	virtual ~DataChange()
	{}

	/*!
	 * Returns the offset of the start of this change in the document.
	 * @return the offset
	 */
	offset_t GetChangeOffset() const
	{
		return m_offset;
	}

	virtual offset_t GetChangeSize() const = 0;

	/*!
	 * Return a pointer to this change's data
	 * @return the data pointer - use GetChangeSize() to find out how
	 * much data there is.
	 *
	 * TODO: return a vector?
	 */
	virtual const char* GetChangeData() const = 0;

private:
	offset_t m_offset;
};

class InsertionChange: public DataChange
{
public:
	/*!
	 * The size of the inserted block
	 * @return
	 */
	offset_t GetChangeSize() const override
	{
		return m_insData.size();
	}

	/*!
	 * The data in the inserted block
	 * @return
	 */
	const char* GetChangeData() const
	{
		return &m_insData.front();
	}

private:

	std::vector<uint8_t> m_insData; // the inserted data
};


#endif /* DAL_DATACHANGE_H_ */
