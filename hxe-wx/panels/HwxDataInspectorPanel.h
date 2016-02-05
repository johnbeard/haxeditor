/*
 * wxDataInspectorPanel.h
 *
 *  Created on: 4 Feb 2016
 *      Author: John Beard
 */

#ifndef HXE_WX_PANELS_HWXDATAINSPECTORPANEL_H_
#define HXE_WX_PANELS_HWXDATAINSPECTORPANEL_H_

#include <wx/panel.h>
#include <wx/checkbox.h>

#include "haxeditor/panels/DataInspectorPanel.h"

/*!
 * Wx Panel for presenting a DataInspectorPanel
 */
class HwxDataInspectorPanel: public wxPanel
{
public:
	HwxDataInspectorPanel(wxWindow* parent, wxWindowID id = wxID_ANY);

	~HwxDataInspectorPanel();

private:
	// handles to controls - PImpl time?
	wxCheckBox* m_cbUnsigned;
};

#endif /* HXE_WX_PANELS_HWXDATAINSPECTORPANEL_H_ */
