/*
 * ChangeStack.h
 *
 *  Created on: 6 Jan 2016
 *      Author: John Beard
 */

#ifndef DAL_CHANGESTACK_H_
#define DAL_CHANGESTACK_H_

#include "DataChange.h"

class ChangeStack
{
public:

private:

	typedef std::vector<DataChange> ChangeList;

	// the ordered list of changes applying to the
	ChangeList m_changeList;

	// the underlying data that these changes are modifying
	DataAbstraction* m_underlyingData;
};



#endif /* DAL_CHANGESTACK_H_ */
