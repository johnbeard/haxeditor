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
	HaxFrame():
		m_caretVisible(true)
	{}

	virtual ~HaxFrame()
	{}

	virtual void SetOffset(uint64_t newOffset) = 0;

	virtual unsigned GetMinimumWidthForData() const = 0;

	virtual void moveCaret() = 0;

protected:

	bool isCaretVisible() const
	{
		return m_caretVisible;
	}

	void setCaretVisible(bool visible)
	{
		m_caretVisible = visible;
	}

private:
	bool m_caretVisible;
};



#endif /* HAXEDITOR_HAXFRAME_H_ */
