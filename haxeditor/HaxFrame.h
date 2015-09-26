/*
 * HaxFrame.h
 *
 *  Created on: 26 Sep 2015
 *      Author: john
 */

#ifndef HAXEDITOR_HAXFRAME_H_
#define HAXEDITOR_HAXFRAME_H_

/*!
 * The main type of data view in haxeditor: a frame filled will some sort
 * of cells, arranged in rows
 */
class HaxFrame
{
public:
	virtual ~HaxFrame()
	{}

	virtual void SetOffset(uint64_t newOffset) = 0;
};



#endif /* HAXEDITOR_HAXFRAME_H_ */
