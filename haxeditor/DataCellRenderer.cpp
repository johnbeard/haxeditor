/*
 * StringDataRenderer.cpp
 *
 *  Created on: 28 Jan 2016
 *      Author: John Beard
 */

#include "DataRenderer.h"

DataCellRenderer::DataCellRenderer(HaxDocument& doc):
		HaxDataRenderer(doc),
		m_width(0)
{}

/*!
 * Gets the width in elements (char width depends on the format...)
 */
offset_t DataCellRenderer::GetWidth() const
{
	return m_width;
}

void DataCellRenderer::SetWidth(offset_t width)
{
	m_width = width;
}

unsigned DataCellRenderer::GetBitsPerChar() const
{
	return GetWidth() / (GetCellsPerRow() * GetCellChars());
}

unsigned DataCellRenderer::GetBitsPerCell() const
{
	return GetCellChars() * GetBitsPerChar();
}

offset_t DataCellRenderer::GetRowForOffset(offset_t offset, bool leftEdge) const
{
	offset_t row = 0;

	if (auto w = GetWidth())
	{
		row = offset / w;

		// end is on a row boundary, but we want the left edge
		if (!leftEdge && (offset % w == 0))
			row -= 1;
	}

	return row;
}

/*!
 * Get the position of an offset in terms of a StringLinePos
 *
 * @param offset the offset to get the pos of
 * @return
 */
DataCellRenderer::StringLinePos DataCellRenderer::GetOffsetPosInLine(
		offset_t offset, bool leftEdge) const
{
	StringLinePos pos;
	const auto w = GetWidth();

	if (!w)
		return pos;

	auto offsetInLine = offset % w;

	if (!leftEdge && (offsetInLine == 0))
	{
		// this means we are asking for the offset on a line boundary
		// we were asked for the right edge, which is the end of the
		// first line
		offsetInLine = w;

		// the left edge is still zero
	}

	const auto nChars = offsetInLine / GetBitsPerChar();
	const auto nCells = nChars / GetCellChars();

	pos.chars = nChars;
	pos.gaps = nCells;

	// if we are on a cell boundary, remove the gap if a right edge is wanted
	// and we are not at the start of the line already
	if (offsetInLine % GetBitsPerCell() == 0
			&& !leftEdge
			&& (offsetInLine != 0))
	{
		pos.gaps -= 1;
	}

	return pos;
}

/*!
 * Get the position of the end of a line (this is the same for all lines)
 * @return the position
 */
DataCellRenderer::StringLinePos DataCellRenderer::GetLineEndPos() const
{
	// special case defer to general case
	return GetOffsetPosInLine(GetWidth(), false);
}

DataCellRenderer::StringLinePos DataCellRenderer::GetLineStartPos() const
{
	// special case defer to general case
	return GetOffsetPosInLine(0, true);
}
