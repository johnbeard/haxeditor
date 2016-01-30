/*
 * HaxTextCellFrame.cpp
 *
 *  Created on: 30 Jan 2016
 *      Author: John Beard
 */

#include "HaxTextCellFrame.h"

#include <iostream>

HaxTextCellFrame::HaxTextCellFrame(StringCellRenderer& renderer,
		HaxPagedDocumentView& docView):
		m_renderer(renderer),
		m_docView(docView)
{
}

DataCellRenderer::StringLinePos HaxTextCellFrame::GetMaximumTextWidth() const
{
	return m_renderer.GetLineEndPos();
}

void HaxTextCellFrame::SetCaretPosition(offset_t newOffset)
{
	const unsigned chPerCell = m_renderer.GetCellChars();
	const unsigned bitsPerChar = m_renderer.GetBitsPerChar();
	const unsigned bitsPerCell = bitsPerChar * chPerCell;

	// work out the new care position
	const unsigned w = m_renderer.GetWidth();
	uint64_t div = newOffset / w; // in rows
	uint64_t rem = newOffset % w; // in bits

	uint64_t cells = rem / bitsPerCell;
	uint64_t chars = (rem % bitsPerCell) / bitsPerChar;

	m_caretPos.row = div;
	m_caretPos.charsX = cells * chPerCell + chars;

	// add the intercell gaps
	if (cells)
		m_caretPos.gapsX = cells;

	//std::cout << "new caret pos for offset " << newOffset << ": "
	//		<< m_caretPos.charsX << ", " << m_caretPos.row << std::endl;
}

std::string HaxTextCellFrame::GetRowString(offset_t row) const
{
	const auto offset = m_docView.GetPageOffset() + row * m_renderer.GetWidth();
	return m_renderer.RenderLine(offset);
}
