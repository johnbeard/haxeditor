/*
 * hexframe.h
 *
 *  Created on: 8 Sep 2015
 *      Author: john
 */

#ifndef HXE_WX_HEXFRAME_H_
#define HXE_WX_HEXFRAME_H_

#include <wx/scrolwin.h>
#include <memory>

#include "haxeditor/haxeditor.h"

class HexFrame: public wxScrolledWindow
{
public:
	HexFrame(wxWindow *parent, wxWindowID id,
			const wxPoint &pos, const wxSize &size,
			const HaxStringRenderer& renderer);

	void DataChanged();

	void Paint(wxPaintEvent& event);

protected:
	wxPoint   m_Margin;	// the margin around the text (looks nicer)
	wxPoint   m_Caret;	// position (in text coords) of the caret
	wxPoint   m_Window;	// the size (in text coords) of the window
	wxString  m_text;

private:
	std::unique_ptr<wxBitmap> m_bmpBuffer;
	const HaxStringRenderer& m_renderer;
};



#endif /* HXE_WX_HEXFRAME_H_ */
