/*
 * hexframe.h
 *
 *  Created on: 8 Sep 2015
 *      Author: john
 */

#ifndef HXE_WX_HEXFRAME_H_
#define HXE_WX_HEXFRAME_H_

#include <wx/wx.h>

#include <wx/scrolbar.h>

#include <memory>

#include "haxeditor/haxeditor.h"

#include "HugeScrollBar.h"

class HexFrame: public wxWindow
{
public:

	class Director
	{
	public:
		virtual const wxSize& GetCharSize() const = 0;
		virtual unsigned GetNumRowsToShow() const = 0;
		virtual const wxTextAttr& GetTextAttr() const = 0;
		virtual	const wxSize& GetFrameMargin() const = 0;
	};

	HexFrame(wxWindow *parent, wxWindowID id,
			const wxPoint &pos, const wxSize &size,
			Director* director,
			const HaxStringRenderer& renderer);

	void DataChanged(bool force);

	void Paint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& /*event*/);

	void SetOffset(uint64_t offset);

	/*!
	 * Return the minimum width needed for the current data
	 */
	unsigned GetMinimumWidthForData() const;

protected:
	wxPoint   m_Caret;	// position (in text coords) of the caret
	wxString  m_text;

private:

	void drawToBitmap(wxDC& dc);

	/*!
	 * The internal state of the view - if this doesn't change, we
	 * don't recalculate anything, save us some redraw effort cycles
	 */
	struct State
	{
		int m_rows;
		int m_cols;
		uint64_t offset;
		wxPoint m_margin; // the margin around the text
		wxSize m_charSize;

		State():
			m_rows(0),
			m_cols(0),
			offset(0)
		{}

		bool redrawNeeded(const State& other) const
		{
			 return offset != other.offset
					 || m_rows != other.m_rows
					 || m_cols != other.m_cols
					 || m_margin != other.m_margin;
		}
	};

	State m_state, m_pendingState;

	wxBitmap m_bmpBuffer;
	const HaxStringRenderer& m_renderer;
	Director* m_director;
};

#endif /* HXE_WX_HEXFRAME_H_ */
