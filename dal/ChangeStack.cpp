/*
 * ChangeStack.cpp
 *
 *  Created on: 20 Jan 2016
 *      Author: John Beard
 */

#include "ChangeStack.h"

#include "utils/CompilerSupport.h" // make unique

#include <iostream>

//#define DEBUG_CHANGE_STACK

/*!
 * Private impl for ChageStack
 */
class ChangeStack::PImpl
{
public:
	typedef std::list<std::unique_ptr<DataChange> > OrigChList;
	OrigChList origChanges;

	/*!
	 * List of the applied changes in order of change start
	 */
	ChList changes;

	/*!
	 * Insert a change into the list
	 *
	 * The change will be inserted as early as possible - it will precede other
	 * changes starting at the same offset. Changes that it invalidates
	 * will be modified or removed as needed
	 *
	 * @param change the change to insert
	 * @return the iterator of the inserted change
	 */
	void insertChange(std::unique_ptr<AppliedChange> change);

	/*!
	 * Check for invalidated changes ocurring before an inserted change
	 */
	void processInvalidationBefore(ChList::iterator inserted);
	void processInvalidationAfter(ChList::iterator inserted);
};

ChangeStack::ChangeStack():
		impl(std::make_unique<PImpl>())
{}


ChangeStack::~ChangeStack()
{}

bool ChangeStack::AddChange(std::unique_ptr<DataChange> newChange,
		offset_t position)
{
	// store the new change in the stack (which takes care of ownership)
	impl->origChanges.push_back(std::move(newChange));

	// we should definitely have this change in the list
	auto& newChangeInList = *impl->origChanges.back().get();

	// construct the shell of the change
	// starts where inserted and has the full initial range within the
	// DataChange provided - a fresh change is always fully applied
	std::unique_ptr<AppliedChange> appCh = std::make_unique<AppliedChange>();

	appCh->change = &newChangeInList;
	appCh->changeOffset = position;
	appCh->start = 0;
	appCh->end = newChangeInList.GetChangeSize();

	// push the change onto the stack and re-compute later changes.
	impl->insertChange(std::move(appCh));

	// TODO what would produce false here?
	return true;
}

const ChangeStack::ChList& ChangeStack::GetChangeList() const
{
	return impl->changes;
}

offset_t ChangeStack::GetLengthModification() const
{
	// TODO: fix this when we support ins/del
	return 0;
}

void ChangeStack::PImpl::insertChange(
		std::unique_ptr<AppliedChange> change)
{
	// if we find no changes the occur before this one, we add at the end
	auto insBeforeThis = changes.end();

	// first - find the place in the list according to the start offset
	// this might be a good place to build a skip list, since this is otherwise quadratic...
	for (auto c = changes.begin(), e = changes.end(); c != e; ++c)
	{
		// the old change starts before the new change
		// so the new change can't go before it
		if ((*c)->changeOffset < change->changeOffset)
			continue;

		insBeforeThis = c;
		break;
	}

#ifdef DEBUG_CHANGE_STACK
	if (insBeforeThis != changes.end())
		std::cout << "Inserting before change of size: " << (*insBeforeThis)->change->GetChangeSize() << std::endl;
	else
		std::cout << "Inserting at list end" << std::endl;
#endif

	// insert the new change at that point
	const auto inserted = changes.insert(insBeforeThis, std::move(change));

	processInvalidationBefore(inserted);

	processInvalidationAfter(inserted);

	// that's it!
	// want to return the new change in the list?
}

void ChangeStack::PImpl::processInvalidationBefore(ChList::iterator inserted)
{
	// if there are no changes before the new change, just bail out
	if (inserted == changes.begin())
		return;

	// working backwards from the new change, find any changes that extend
	// under the new change, and deal with them accordingly
	for (auto c = inserted, e = changes.begin(); c-- != e; )
	{
		// change end-points in absolute terms
		const auto oldChEnd = (*c)->changeOffset + (*c)->end;
		const auto newChStart = (*inserted)->changeOffset + (*inserted)->start;
		const auto newChEnd = (*inserted)->changeOffset + (*inserted)->end;

		if (oldChEnd <= newChStart)
		{
			// the old change ends before the inserted change: one of:
			// ....1111......
			// .........222..
			// ........222...

			// we have reached the end of the preceding changes that the new
			// change has modified
			break;
		}

		// if we get here, the new change has modified a preceding existing change

		// we know the new change has a start greater than any preceding change
		// or (it would have been inserted before them)

		if (newChEnd < oldChEnd)
		{
			// the old change contains the new change
			// which means the new change splits the old one

			// ....111111....
			// .....22....... (splits)
			// ....122111....

			// create the second half of the old change
			std::unique_ptr<AppliedChange> tailCh = std::make_unique<AppliedChange>();

			tailCh->change = (*c)->change; // change buffer is the same
			tailCh->changeOffset = (*c)->changeOffset; // the internal change data is still referred to the same origin
			tailCh->start = newChEnd - tailCh->changeOffset; // starts where the new change ends
			tailCh->end = (*c)->end; // end is the same as the old one

			// now modify the original change, which is now just the first segment
			(*c)->end = newChStart - (*c)->changeOffset;
		}
		else
		{
			// ....111111....
			// .....2222222.. (truncates)
			// ....12222222..

			// so set the old end to the new start w.r.t. the old offset
			(*c)->end = newChStart - (*c)->changeOffset;
		}
	}
}

void ChangeStack::PImpl::processInvalidationAfter(ChList::iterator inserted)
{
	// and now check for later changes that the new change will invalidate
	for (auto c = std::next(inserted), e = changes.end(); c != e; ++c)
	{
		// change end-points in absolute terms
		const auto oldChStart = (*c)->changeOffset + (*c)->start;
		const auto oldChEnd = (*c)->changeOffset + (*c)->end;
		const auto newChEnd = (*inserted)->changeOffset + (*inserted)->end;

		if (oldChStart >= newChEnd)
		{
			// the existing change starts after the new change ends, so
			// it can't be affected
			// TODO unless the new change changes length, then we might need
			// to adjust all offsets right up to the list end?
			break;
		}
		else
		{
			// in here, the existing change has been modified - but how?

			if (oldChEnd > newChEnd)
			{
				// the existing change extends past the end of the new change
				// ....11111111....
				// ..22222......... (truncate from front)
				// ..2222211111....

				// so set the old end to the new start w.r.t. the old offset
				(*c)->start = newChEnd - (*c)->changeOffset;
			}
			else
			{
				// the old change is contained entirely in the new change
				// so it is totally invalidated
				// ....111....
				// ..2222222.... (removes)
				// ..2222222....

				// remove the old change
				c = changes.erase(c);

				// the iterator now refers to the change following the erased change
				// but we want check that on the next pass
				c = std::prev(c);
			}
		}
	}
}
