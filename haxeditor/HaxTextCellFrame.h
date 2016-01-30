/*
 * HaxTextCellFrame.h
 *
 *  Created on: 30 Jan 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_HAXTEXTCELLFRAME_H_
#define HAXEDITOR_HAXTEXTCELLFRAME_H_

#include "HaxFrame.h"
#include "StringCellRenderer.h"
#include "HaxPagedDocumentView.h"

/*!
 * A HaxTextCellFrame is a kind of data frame that shows a grid of some sort
 * of textual data (REVIEW: or can it be any gridded data?)
 *
 * These are often found in the "hex" and "text" areas of the editor
 */
class HaxTextCellFrame: public HaxFrame
{
public:
	HaxTextCellFrame(StringCellRenderer& renderer,
			HaxPagedDocumentView& docView);

	void SetCaretPosition(offset_t newOffset) override;

	/*!
	 * @return the characteristics of a row in terms of chars and gaps
	 */
	DataCellRenderer::StringLinePos GetMaximumTextWidth() const;

	/*!
	 * Get the row'th full line string
	 * // TOOD implement as cells?
	 * @param row
	 * @return
	 */
	std::string GetRowString(offset_t row) const;

	offset_t GetPageOffset() const
	{
		return m_docView.GetPageOffset();
	}

//protected:
	struct CaretPos
	{
		int charsX = 0;
		int gapsX = 0;
		int row = 0;
	};

	CaretPos   m_caretPos;	// position (in text coords) of the caret

private:
	StringCellRenderer& m_renderer;
	HaxPagedDocumentView& m_docView;
};



#endif /* HAXEDITOR_HAXTEXTCELLFRAME_H_ */
