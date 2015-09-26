/*
 * HexMultiFrame.h
 *
 *  Created on: 15 Sep 2015
 *      Author: John Beard
 */

#ifndef HXE_WX_HEXMULTIFRAME_H_
#define HXE_WX_HEXMULTIFRAME_H_

#include <wx/wx.h>
#include "hexframe.h"
#include "haxeditor/HaxDocumentMultiFrame.h"

class HexMultiFrame: public wxScrolledWindow,
					 public HaxDocumentMultiFrame,
					 public HexFrame::Director
{
public:
	HexMultiFrame(wxWindow* parent, wxWindowID id, HaxDocument& m_doc);

	~HexMultiFrame();

	void OnFontChange();
	void OnResize(wxSizeEvent& event);
	void OnOffsetScroll(wxScrollEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnKeyboardInput(wxKeyEvent& event);

	// Interface to data windows HexFrame::Director
	// TODO composite this interface rather than inherit?
	const wxSize& GetCharSize() const override
	{
		return m_charSize;
	}

	/*!
	 * Get the (default) border around the edges of the frame
	 * Frames can override it, especially for the left/right edges, but
	 * the top should be the same for all windows to keep the data in line
	 */
	const wxSize& GetFrameMargin() const override
	{
		return m_frameMargin;
	}

	const wxTextAttr& GetTextAttr() const override
	{
		return m_textAttr;
	}

private:

	// forward to the HaxDocumentMultiFrame
	unsigned GetNumRowsToShow() const override
	{
		return GetNumVisibleRows();
	}

	/*!
	 * Internal function to recompute the scrollbar position and thumb sizing
	 *
	 * Called for example when the multiframe window is resized
	 */
	void AdjustScrollBar();

	/*!
	 * Called when something has changed the offset
	 */
	void updateOffset() override;

	HexFrame* m_addrFrame, *m_hexFrame, *m_textFrame;
	wxSizer* m_hexPanelSizer;

	std::unique_ptr<HaxStringRenderer> m_hexRenderer;
	std::unique_ptr<HaxAddressRenderer> m_addrRenderer;
	std::unique_ptr<HaxTextRenderer> m_textRenderer;

	wxScrollBar* m_realScrollBar;
	wxHugeScrollBar* m_hugeScrollBar;

	wxSize m_charSize;
	wxTextAttr m_textAttr;
	wxSize m_frameMargin;

};


#endif /* HXE_WX_HEXMULTIFRAME_H_ */
