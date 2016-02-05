/*
 * DataPanel.h
 *
 *  Created on: 1 Feb 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_PANELS_DATAPANEL_H_
#define HAXEDITOR_PANELS_DATAPANEL_H_

#include <string>

/*!
 * A DataPanel is a class that represents some interpretation of the data.
 * This is a very generic concept and includes things like a "data inspector",
 * checksums, summaries, etc
 */
class DataPanel
{
public:
	virtual ~ DataPanel()
	{}

	/*!
	 * Gets the title of a given panel for display
	 */
	virtual std::string GetPanelTitle() const = 0;
};



#endif /* HAXEDITOR_PANELS_DATAPANEL_H_ */
