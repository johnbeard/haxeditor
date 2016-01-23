/*
 * ChangeStack.h
 *
 *  Created on: 6 Jan 2016
 *      Author: John Beard
 */

#ifndef DAL_CHANGESTACK_H_
#define DAL_CHANGESTACK_H_

#include "DataChange.h"

#include <list>
#include <memory>

// TODO hide this?
struct AppliedChange
{
	/*!
	 * The change itself
	 */
	DataChange* change = nullptr;

	/*!
	 * The offset of byte 0 of the change from byte 0 of the document
	 */
	offset_t changeOffset = 0;

	/*!
	 * The start and end of the useful parts of the change.
	 *
	 * Note that these are measured from the start of the change, so you should
	 * add them to that if you are considering the start in the context of the
	 * whole document
	 */
	offset_t start = 0;
	offset_t end = 0;
};

/*!
 * Wrapper around a list of applied changes which handles adding new changes
 * and removing obsolete ones.
 *
 * TODO also handle resetting?
 */
class ChangeStack
{
public:

	ChangeStack();
	~ChangeStack();

	/*!
	 * Ordered list of applied changes
	 */
	typedef std::list<std::unique_ptr<AppliedChange> > ChList;

	bool AddChange(std::unique_ptr<DataChange> newChange, offset_t position);

	/*!
	 * Get the list of changes, in order of occurrence. The applied changes
	 * in this list do not overlap, and occur in order of the start of the
	 * applied data in terms of the overall document
	 *
	 * @return the list of changes
	 */
	const ChList& GetChangeList() const;

	/*!
	 * Returns the total amount that the whole change stack changes the
	 * data length by
	 * @return the different in BYTES
	 */
	offset_t GetLengthModification() const;

private:

	class PImpl;
	std::unique_ptr<PImpl> impl;
};

#if 0
/*!
 * Returns the change that data at an offset comes from
 *
 * THis will return the whole applied change, including the base offset of
 * the change and the start and end of the active area.
 *
 * This me
 * @param offset
 */
AppliedChange* GetChangeForOffset(offset_t offset) const;
#endif

#endif /* DAL_CHANGESTACK_H_ */
