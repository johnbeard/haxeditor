/*
 * hexframe.h
 *
 *  Created on: 8 Sep 2015
 *      Author: john
 */

#ifndef HXE_WX_HEXFRAME_H_
#define HXE_WX_HEXFRAME_H_

#include <wx/scrolwin.h>
#include <wx/textctrl.h>

#include <memory>

#include "haxeditor/haxeditor.h"

class HexFrame: public wxScrolledWindow
{
public:
	HexFrame(wxWindow *parent, wxWindowID id,
			const wxPoint &pos, const wxSize &size,
			const HaxStringRenderer& renderer);

	void DataChanged(bool force);

	void Paint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& /*event*/);

protected:
	wxPoint   m_Margin;	// the margin around the text (looks nicer)
	wxPoint   m_Caret;	// position (in text coords) of the caret
	wxString  m_text;

private:

	/*!
	 * The internal state of the view - if this doesn't change, we
	 * don't recalculate anything, save us some redraw effort cycles
	 */
	struct State
	{
		int m_rows;
		int m_cols;
		uint64_t offset;

		State():
			m_rows(0),
			m_cols(0),
			offset(0)
		{}

		bool operator==(const State& other) const
		{
			 return offset == other.offset
					 && m_rows == other.m_rows
					 && m_cols == other.m_cols;
		}
	};

	State m_state;

	wxBitmap m_bmpBuffer;
	wxTextAttr m_textAttr;
	const HaxStringRenderer& m_renderer;
};



#endif /* HXE_WX_HEXFRAME_H_ */
