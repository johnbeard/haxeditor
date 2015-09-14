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
	HexFrame(wxWindow *parent, wxWindowID id,
			const wxPoint &pos, const wxSize &size,
			const HaxStringRenderer& renderer);

	void DataChanged(bool force);

	void Paint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& /*event*/);

	void SetOffset(uint64_t offset);

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
	wxTextAttr m_textAttr;
	const HaxStringRenderer& m_renderer;
};

class HexMultiFrame: public wxScrolledWindow
{
public:
	HexMultiFrame(wxWindow* parent, wxWindowID id, HaxDocument& m_doc):
		wxScrolledWindow(parent, id),
		m_rowOffset(0)
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

		m_realScrollBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition,
				wxDefaultSize, wxSB_VERTICAL);
		m_realScrollBar->Enable(true);

		m_realScrollBar->Bind( wxEVT_SCROLL_TOP, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Bind( wxEVT_SCROLL_BOTTOM, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Bind( wxEVT_SCROLL_LINEUP, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Bind( wxEVT_SCROLL_LINEDOWN, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Bind( wxEVT_SCROLL_PAGEUP, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Bind( wxEVT_SCROLL_PAGEDOWN, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Bind( wxEVT_SCROLL_THUMBTRACK, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Bind( wxEVT_SCROLL_THUMBRELEASE, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Bind( wxEVT_SCROLL_CHANGED, &HexMultiFrame::OnOffsetScroll, this );

		// do this AFTER the real scrollbar binding (so it will have priotity),
		// because we need the HugeScrollBar to get its position before we deal
		// with the event though the wrapper in this call!
		m_hugeScrollBar = new wxHugeScrollBar( m_realScrollBar );

		m_hexPanelSizer = new wxBoxSizer(wxHORIZONTAL);
		SetSizer(m_hexPanelSizer);

		m_hexPanelSizer->Add(m_addrFrame, 0, wxALIGN_RIGHT | wxEXPAND, 10);
		m_hexPanelSizer->Add(m_hexFrame, 60, wxALIGN_RIGHT | wxEXPAND, 10);
		m_hexPanelSizer->Add(m_textFrame, 40, wxALIGN_RIGHT | wxEXPAND, 10);
		m_hexPanelSizer->Add(m_realScrollBar, 0, wxALIGN_RIGHT | wxEXPAND, 0);

		m_hexPanelSizer->Show(true);

		// set text size
		m_textAttr = wxTextAttr(
				wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ),
				wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ),
				wxFont(
						10,
						wxFONTFAMILY_MODERN,	// family
						wxFONTSTYLE_NORMAL,	// style
						wxFONTWEIGHT_NORMAL,// weight
						false,				// underline
						wxT(""),			// facename
						wxFONTENCODING_UTF8
				)
		);

		OnFontChange();

		Bind(wxEVT_SIZE, &HexMultiFrame::OnResize, this);
	}

	~HexMultiFrame()
	{
		m_realScrollBar->Unbind( wxEVT_SCROLL_TOP, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Unbind( wxEVT_SCROLL_BOTTOM, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Unbind( wxEVT_SCROLL_LINEUP, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Unbind( wxEVT_SCROLL_LINEDOWN, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Unbind( wxEVT_SCROLL_PAGEUP, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Unbind( wxEVT_SCROLL_PAGEDOWN, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Unbind( wxEVT_SCROLL_THUMBTRACK, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Unbind( wxEVT_SCROLL_THUMBRELEASE, &HexMultiFrame::OnOffsetScroll, this );
		m_realScrollBar->Unbind( wxEVT_SCROLL_CHANGED, &HexMultiFrame::OnOffsetScroll, this );

		Unbind(wxEVT_SIZE, &HexMultiFrame::OnResize, this);
	}

	void OnFontChange()
	{
		wxMemoryDC mdc;
		mdc.SetFont(m_textAttr.GetFont());
		m_charSize = wxSize(mdc.GetCharWidth(), mdc.GetCharHeight());
	}

	const wxSize& GetCharSize() const
	{
		return m_charSize;
	}

	void AdjustScrollBar()
	{
		const uint64_t dataSize = 303;
		const unsigned lineSize = 10;

		uint64_t dataRows = dataSize / lineSize;
		const unsigned lastRowCount = dataSize % lineSize;

		if (lastRowCount)
			dataRows += 1;

		const int paneH = GetSize().GetHeight();

		std::cout << "Adjust SB to fit window height: " << paneH << std::endl;

		const int usableH = paneH - 3;
		// could use a different line spacing
		const int rowH = m_charSize.GetHeight() + 3;

		const int m_rows = (usableH / rowH) + 1;

		m_hugeScrollBar->SetScrollbar(m_rowOffset, m_rows, dataRows, m_rows - 1, true);
	}

	void OnResize(wxSizeEvent& event)
	{
		std::cout << "Resize: " << event.GetSize().GetWidth() << ", "
				<< event.GetSize().GetHeight() << std::endl;
		// recompute the scrollbar properties
		AdjustScrollBar();
	}

	void OnOffsetScroll(wxScrollEvent& /*event*/)
	{
		m_rowOffset = m_hugeScrollBar->GetThumbPosition() ;
		std::cout << "hmf offset scroll " << m_rowOffset << std::endl;

		m_addrFrame->SetOffset(m_rowOffset);
		m_hexFrame->SetOffset(m_rowOffset);
		m_textFrame->SetOffset(m_rowOffset);
	}

private:
	HexFrame* m_addrFrame, *m_hexFrame, *m_textFrame;
	wxSizer* m_hexPanelSizer;

	std::unique_ptr<HaxStringRenderer> m_hexRenderer;
	std::unique_ptr<HaxAddressRenderer> m_addrRenderer;
	std::unique_ptr<HaxTextRenderer> m_textRenderer;

	wxScrollBar* m_realScrollBar;
	wxHugeScrollBar* m_hugeScrollBar;

	uint64_t m_rowOffset;
	wxSize m_charSize;
	wxTextAttr m_textAttr;
};


#endif /* HXE_WX_HEXFRAME_H_ */
