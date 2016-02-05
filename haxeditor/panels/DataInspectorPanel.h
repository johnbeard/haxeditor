/*
 * DataInspectorPanel.h
 *
 *  Created on: 1 Feb 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_PANELS_DATAINSPECTORPANEL_H_
#define HAXEDITOR_PANELS_DATAINSPECTORPANEL_H_

#include "haxeditor/panels/DataPanel.h"
#include "haxeditor/inspection/DataInspector.h"

#include "haxeditor/HaxDocument.h"

/*!
 * A DataInspectorControl is a class that holds a single DataInspector and
 * provides the interface for setting it up and displaying the result.
 *
 * It could be used singly, or in a group with additional global controls
 */
class DataInspectorControl
{
public:

private:
	std::unique_ptr<DataInspector> m_inspector;
};

/*!
 * A data inspector panel is a panel that contains multiple "data inspectors",
 * which shot multiple interpretations of a chunk of data, and this panel
 * provides ways to add, remove, modify these inspectors, jointly or individually
 */
class DataInspectorPanel: public DataPanel
{
public:
	DataInspectorPanel(HaxDocument& doc):
		m_doc(doc)
	{}

	std::string GetPanelTitle() const override;

private:

	void setDefaultInspectors();

	std::vector<DataInspectorControl> m_inspectors;

	HaxDocument& m_doc;
};


#endif /* HAXEDITOR_PANELS_DATAINSPECTORPANEL_H_ */
