/*
 * StringCellRenderer.h
 *
 *  Created on: 28 Jan 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_STRINGCELLRENDERER_H_
#define HAXEDITOR_STRINGCELLRENDERER_H_

#include "DataRenderer.h"

/*!
 * Class which provides a string output from a cell-based renderer as "lines"
 * (so all cells in a row get combined into a single "line" string
 *
 * The common basic "hex" and "text" views are examples of this
 */
class StringCellRenderer: public DataCellRenderer
{
protected:
	StringCellRenderer(HaxDocument& doc):
		DataCellRenderer(doc)
	{}

public:
	virtual std::string RenderLine(offset_t offset) const = 0;
};

/*!
 * Basic hex renderer which provides cells of single bytes
 */
class HaxHexRenderer: public StringCellRenderer
{
public:
	HaxHexRenderer(HaxDocument& doc);

	std::string RenderLine(offset_t offset) const override;

	unsigned GetCellsPerRow() const override;
	unsigned GetCellChars() const override;
	CellList GetRowCells(offset_t /*offset*/) const override;
};


class HaxTextRenderer: public StringCellRenderer
{
public:
	HaxTextRenderer(HaxDocument& doc);

	std::string RenderLine(offset_t offset) const override;

	unsigned GetCellsPerRow() const override;
	unsigned GetCellChars() const override;
	CellList GetRowCells(offset_t /*offset*/) const override;

private:

	void addCharToString(const char c, std::stringstream& ss) const;
};


class HaxAddressRenderer: public StringCellRenderer
{
public:
	HaxAddressRenderer(HaxDocument& doc);

	std::string RenderLine(offset_t offset) const override;
	unsigned GetCellsPerRow() const override;
	unsigned GetCellChars() const override;
	CellList GetRowCells(offset_t offset) const override;

private:

	unsigned getAddrWidth() const;
};

#endif /* HAXEDITOR_STRINGCELLRENDERER_H_ */
