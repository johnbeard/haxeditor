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

class HexFrame: public wxScrolledWindow
{
public:
	HexFrame(wxWindow *parent, wxWindowID id,
			const wxPoint &pos, const wxSize &size);

	void SetData(char* data, uint64_t len);

	void Paint(wxPaintEvent& event);

protected:
	wxPoint   m_Margin;	// the margin around the text (looks nicer)
	wxPoint   m_Caret;	// position (in text coords) of the caret
	wxPoint   m_Window;	// the size (in text coords) of the window
	wxString  m_text;

	// TODO replace with a proper buffer interface
	char* m_data;
	uint64_t m_len;

private:
	std::unique_ptr<wxBitmap> internalBmp;
};



#endif /* HXE_WX_HEXFRAME_H_ */
