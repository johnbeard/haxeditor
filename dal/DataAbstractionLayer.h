/*
 * DataAbstractionLayer.h
 *
 *  Created on: 22 Dec 2015
 *      Author: John Beard
 */

#ifndef DAL_DATAABSTRACTIONLAYER_H_
#define DAL_DATAABSTRACTIONLAYER_H_

#include <dal/DalTypes.h>

/*!
 * The DataAbstraction class is the base class for all data "files" handled
 * by HaxEditor.
 *
 * It consists of some "base" (for example, a file), along with a set of
 * "changes" made to it. It can also provide ways to write this changed version
 * back, but this is not always possible if the data source is read only.
 *
 * You will need to provide an actual implementation of a DataAbstraction to
 * be able to use it to represent data.
 */
class DataAbstraction
{
public:
	/*!
	 * Public virtual dtor - these can reasonable be held by base pointer.
	 */
	virtual ~DataAbstraction()
	{}

	/*!
	 * Gets the length of the data right now, including any changes made.
	 */
	datasize_t GetCurrentDataLength() const;

	/*!
	 * Returns the length of the "base" data, without considering any changes
	 * made. Bear in mind that "saving" the data will cause this to change.
	 */
	virtual datasize_t GetBaseDataLength() const = 0;

	/*!
	 * Returns a pointer to the data at the offset (with changes applied)
	 *
	 * TODO: return some sort of iterator, not a pointer
	 */
	virtual const uint8_t* GetDataAtOffset(offset_t offset) const = 0;

	/*!
	 * Returns if this DA is read-only and can't save data back.
	 */
	virtual bool IsReadOnly() const = 0;

	/*!
	 * Save the data to whatever persistent storage is appropriate. Note that
	 * not all DataAbstractions can save data back.
	 *
	 * @return true if saved OK
	 */
	bool SaveData()
	{
		if (IsReadOnly())
			return false;

		return doSave();
	}

private:

	/*!
	 * Actually execute the save. Implement if the derived DA can save out.
	 *
	 * @return if the save was successful
	 */
	virtual bool doSave()
	{
		return false;
	}

};

#include <vector>

class MemoryDataAbstraction: public DataAbstraction
{
public:

	/*!
	 * Initialise as empty buffer
	 */
	MemoryDataAbstraction()
	{
	}

	/*!
	 * Initialise from some generic data buffer
	 */
	MemoryDataAbstraction(uint8_t* data, datasize_t size)
	{
		m_buffer.insert(m_buffer.end(), data, data + size);
	}

	/*!
	 * Memory DAs cannot be saved - there is nowhere to save them to!
	 */
	bool IsReadOnly() const override
	{
		return true;
	}

	datasize_t GetBaseDataLength() const override
	{
		return m_buffer.size() * BYTE;
	}

	const uint8_t* GetDataAtOffset(offset_t offset) const override
	{
		return &m_buffer[offset / BYTE];
	}

private:
	/*!
	 * The main buffer backing the memory DA (this is the _base_ buffer)
	 * and does not include changes
	 */
	std::vector<uint8_t> m_buffer;
};

#endif /* DAL_DATAABSTRACTIONLAYER_H_ */
