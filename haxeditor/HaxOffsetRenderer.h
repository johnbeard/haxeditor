/*
 * HaxOffsetRenderer.h
 *
 *  Created on: 26 Nov 2015
 *      Author: john
 */

#ifndef HAXEDITOR_HAXOFFSETRENDERER_H_
#define HAXEDITOR_HAXOFFSETRENDERER_H_

#include "haxeditor.h"

/*!
 * Base class for offset renderers
 */
class HaxOffsetRenderer
{
public:
	virtual ~HaxOffsetRenderer()
	{}

	void SetOffset(offset_t newOffset)
	{
		offset = newOffset;
	}

	/*!
	 * Implement to render the offset out to a string
	 * @return
	 */
	virtual std::string RenderOffset() const = 0;

protected:
	offset_t getOffset() const
	{
		return offset;
	}

private:
	offset_t offset = 0;
};

class HaxOffsetRendererDecimal: public HaxOffsetRenderer
{
public:
	std::string RenderOffset() const override
	{
		std::stringstream ss;
		ss << getOffset();

		return ss.str();
	}
};




#endif /* HAXEDITOR_HAXOFFSETRENDERER_H_ */
