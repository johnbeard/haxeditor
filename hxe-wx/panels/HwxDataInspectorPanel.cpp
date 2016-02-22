/*
 * HxeDataInspectorPanel.cpp
 *
 *  Created on: 4 Feb 2016
 *      Author: John Beard
 */

#include <wx/sizer.h>

#include <hxe-wx/panels/HwxDataInspectorPanel.h>
#include "utils/CompilerSupport.h"

HwxDataInspectorPanel::HwxDataInspectorPanel(DataInspectorPanel& corePanel,
		wxWindow* parent, wxWindowID id):
	wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL),
	m_corePanel(corePanel)
{
	// listen for panel events
	m_corePanel.signal_inspectorsChanged.connect(sigc::mem_fun(
			this, &HwxDataInspectorPanel::onPanelsChanged));

	auto mainSizer = std::make_unique<wxBoxSizer>(wxVERTICAL);

	auto globalOptSize = std::make_unique<wxBoxSizer>(wxHORIZONTAL);

	{
		auto cbUnsigned = std::make_unique<wxCheckBox>(this, wxID_ANY, "Unsigned");
		m_cbUnsigned = cbUnsigned.get();
		globalOptSize->Add(cbUnsigned.release());
	}

	mainSizer->Add(globalOptSize.release());

	// initialise the panels
	onPanelsChanged();

	SetSizer(mainSizer.release());
	Layout();
}

HwxDataInspectorPanel::~HwxDataInspectorPanel()
{}

void HwxDataInspectorPanel::onPanelsChanged()
{
}
