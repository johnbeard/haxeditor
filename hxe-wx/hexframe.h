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
#include <wx/sizer.h>

#include <memory>

#include "haxeditor/haxeditor.h"

class HexFrame: public wxWindow
{
public:
	HexFrame(wxWindow *parent, wxWindowID id,
			const wxPoint &pos, const wxSize &size,
			const HaxStringRenderer& renderer);

	void DataChanged(bool force);

	void Paint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& /*event*/);

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
			 return offset == other.offset
					 && m_rows == other.m_rows
					 && m_cols == other.m_cols
					 && m_margin == other.m_margin;
		}
	};

	State m_state;

	wxBitmap m_bmpBuffer;
	wxTextAttr m_textAttr;
	const HaxStringRenderer& m_renderer;
};

class HexMultiFrame: public wxScrolledWindow
{
public:
	HexMultiFrame(wxWindow* parent, wxWindowID id, HaxDocument& m_doc):
		wxScrolledWindow(parent, id)
	{
		const int lineSize = 10;

		m_hexRenderer.reset(new HaxHexRenderer(m_doc));
		m_hexRenderer->SetWidth(lineSize);

		m_addrRenderer.reset(new HaxAddressRenderer(m_doc));
		m_addrRenderer->SetWidth(lineSize);

		m_textRenderer.reset(new HaxTextRenderer(m_doc));
		m_textRenderer->SetWidth(lineSize);

		const int height = 300;

		m_hexFrame = new HexFrame(this, wxID_ANY, wxDefaultPosition, wxSize(300, height),
				*m_hexRenderer);

		m_addrFrame = new HexFrame(this, wxID_ANY, wxDefaultPosition, wxSize(100, height),
				*m_addrRenderer);

		m_textFrame = new HexFrame(this, wxID_ANY, wxDefaultPosition, wxSize(100, height),
				*m_textRenderer);

		wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
		SetSizer(sz);

		sz->Show(true);

		sz->Add(m_addrFrame, 0, wxALIGN_RIGHT | wxEXPAND, 10);
		sz->Add(m_hexFrame, 60, wxALIGN_RIGHT | wxEXPAND, 10);
		sz->Add(m_textFrame, 40, wxALIGN_RIGHT | wxEXPAND, 10);
	}

private:
	HexFrame* m_addrFrame, *m_hexFrame, *m_textFrame;

	std::unique_ptr<HaxStringRenderer> m_hexRenderer;
	std::unique_ptr<HaxAddressRenderer> m_addrRenderer;
	std::unique_ptr<HaxTextRenderer> m_textRenderer;
};


#endif /* HXE_WX_HEXFRAME_H_ */
