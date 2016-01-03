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

class HaxRangeRenderer
{
public:

	/*!
	 * Set the renderer to use for the offsets.
	 * The same renderer is used for both offsets.
	 * @param the new renderer - ownership taken
	 */
	void SetRenderer(HaxOffsetRenderer* newRenderer)
	{
		m_renderer = newRenderer;
	}

	/*!
	 * Update the range of this renderer
	 * @param newStart
	 * @param newEnd
	 */
	void SetRange(offset_t newStart, offset_t newEnd)
	{
		start = newStart;
		end = newEnd;
	}

	/*!
	 * Render the range out to a string
	 */
	std::string RenderRange() const
	{
		std::stringstream ss;

		if (start == end)
		{
			ss << renderNoSelectionMessage();
		}
		else if (m_renderer)
		{
			m_renderer->SetOffset(start);
			ss << m_renderer->RenderOffset();

			ss << " : ";

			m_renderer->SetOffset(end - 1); // don't include the end point!
			ss << m_renderer->RenderOffset();
		}

		return ss.str();
	}

	std::string RenderRangeSize() const
	{
		std::stringstream ss;

		if (start == end)
		{
			ss << renderNoSelectionMessage();
		}
		else if (m_renderer)
		{
			const auto size = end - start;
			m_renderer->SetOffset(size);
			ss << m_renderer->RenderOffset();
		}

		return ss.str();
	}

private:

	std::string renderNoSelectionMessage() const
	{
		return "No selection";
	}

	// external renderer provided by the calling code
	HaxOffsetRenderer* m_renderer;

	offset_t start = 0;
	offset_t end = 0;
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
