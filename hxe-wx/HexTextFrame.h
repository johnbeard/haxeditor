/*
 * hexframe.h
 *
 *  Created on: 8 Sep 2015
 *      Author: John Beard
 */

#ifndef HXE_WX_HEXTEXTFRAME_H_
#define HXE_WX_HEXTEXTFRAME_H_

#include <wx/wx.h>

#include <wx/scrolbar.h>
#include <wx/caret.h>

#include <memory>

#include "haxeditor/HaxFrame.h"
#include "haxeditor/StringCellRenderer.h"
#include "haxeditor/SelectionRenderer.h"
#include "haxeditor/HaxTextCellFrame.h"

#include "HugeScrollBar.h"

class HexTextFrame: public wxWindow
{
public:

	/*!
	 * Class to tell a text frame how to draw itself
	 */
	class Director
	{
	public:
		virtual const wxSize& GetCharSize() const = 0;
		virtual unsigned GetNumRowsToShow() const = 0;
		virtual const wxTextAttr& GetTextAttr() const = 0;
		virtual	const wxSize& GetFrameMargin() const = 0;
	};

	HexTextFrame(wxWindow *parent, wxWindowID id,
			const wxPoint &pos, const wxSize &size,
			Director* director,
			StringCellRenderer& renderer,
			HaxTextCellFrame& textFrame);
	~HexTextFrame();

	void DataChanged(bool force);

	void Paint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);

	/*!
	 * Return the minimum width needed for the current data
	 */
	unsigned GetMinimumWidthForData() const;

private:

	void onSelectionChanged(const HaxDocument::Selection& selection, bool active);
	void onOffsetChanged(offset_t offset);

	void OnKeyboardInput(wxKeyEvent& event);
	void OnLeftMouseDown(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnFocusSet(wxFocusEvent& event);

	void drawToBitmap(wxDC& dc);

	void moveCaret();

	void onCharSizeChanged();

	/*!
	 * The internal state of the view - if this doesn't change, we
	 * don't recalculate anything, save us some redraw effort cycles
	 */
	struct State
	{
		int m_rows;
		uint64_t offset;
		wxPoint m_margin; // the margin around the text
		wxSize m_charSize;
		bool m_selectionActive;

		State():
			m_rows(0),
			offset(0),
			m_selectionActive(false)
		{}

		bool redrawNeeded(const State& other) const
		{
			 return offset != other.offset
					 || m_rows != other.m_rows
					 || m_margin != other.m_margin
					 || m_selectionActive != other.m_selectionActive;
		}
	};

	State m_state, m_pendingState;

	wxBitmap m_bmpBuffer;
	Director* m_director;
	class HexCaret* m_caret;
	wxColour m_bgColour;

	// the render to use for the selection
	SelectionPathRenderer m_selectionRenderer;
	wxPointList m_selectionPolygon;

	HaxTextCellFrame& m_textFrame;
};

#endif /* HXE_WX_HEXTEXTFRAME_H_ */
