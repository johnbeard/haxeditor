/*
 * DataRenderer.h
 *
 *  Created on: 28 Jan 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_DATARENDERER_H_
#define HAXEDITOR_DATARENDERER_H_

#include "haxeditor/haxeditor.h" // for HaxDocument

class HaxDataRenderer
{
public:
	HaxDataRenderer(HaxDocument& doc):
		m_doc(doc)
	{
	}

	virtual ~HaxDataRenderer()
	{}

protected:

	HaxDocument& getDocument() const
	{
		return m_doc;
	}

private:
	HaxDocument& m_doc;
};

/*!
 * Renderer representing an array of "cells" made up of "chars".
 *
 * Whether to actually treat these as textual chars is up to the implementation,
 * but this class will provide the grid calculations
 */
class DataCellRenderer: public HaxDataRenderer
{
public:
	DataCellRenderer(HaxDocument& doc);

	/*!
	 * Gets the width in elements (char width depends on the format...)
	 */
	offset_t GetWidth() const;

	void SetWidth(offset_t width);

	virtual unsigned GetCellsPerRow() const = 0;

	/*!
	 * Get the chars per cell
	 *
	 * Eg XX XX XX XX is 2 chars
	 *    YYYY YYYY   is 4 chars
	 */
	virtual unsigned GetCellChars() const = 0;

	unsigned GetBitsPerChar() const;
	unsigned GetBitsPerCell() const;

	offset_t GetRowForOffset(offset_t offset, bool leftEdge) const;
	/*!
	 * Struct used to define position within a string renderer line. This is
	 * made up of some number of "chars" and some number of "gaps". It's up to
	 * the displayer what these mean
	 */
	struct StringLinePos
	{
		unsigned chars = 0;
		unsigned gaps = 0;
	};

	/*!
	 * Get the position of an offset in terms of a StringLinePos
	 *
	 * @param offset the offset to get the pos of
	 * @return
	 */
	StringLinePos GetOffsetPosInLine(offset_t offset, bool leftEdge) const;

	/*!
	 * Get the position of the end of a line (this is the same for all lines)
	 * @return the position
	 */
	StringLinePos GetLineEndPos() const;

	StringLinePos GetLineStartPos() const;

	/*!
	 * List of cell lengths within a row
	 * Entries are the lengths of cells
	 */
	typedef std::vector<int> CellList;

	/*!
	 * Get the cells (and their lengths) for a row in the renderer.
	 */
	virtual CellList GetRowCells(offset_t rowOffset) const = 0;

private:

	// the width of the renderer, in bits
	offset_t m_width;
};

#endif /* HAXEDITOR_DATARENDERER_H_ */
