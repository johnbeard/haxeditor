/*
 * SelectionRenderer.h
 *
 *  Created on: 9 Nov 2015
 *      Author: john
 */

#ifndef HXE_WX_SELECTIONRENDERER_H_
#define HXE_WX_SELECTIONRENDERER_H_

#include "haxeditor/haxeditor.h"

#include <algorithm>

/*!
 * Renders a selection of textual data. THis is a fairly abstract base, as
 * selection types can vary greatly (e.g. block-wise for a TUI or as an outline
 * at the pixel level for GUIs)
 */
class SelectionRenderer
{
public:

	// TODO: this should go somewhere and be shared by the renderer and
	// this class
	struct Layout
	{
	public:
		int charW, charH;
		int interCellGap;
		int interRowGap;

		int GetXForCell(unsigned cells)
		{
			auto gaps = cells ? (cells - 1) : 0;
			return (charW * cells) + (interCellGap * gaps);
		}

		int GetYForRow(unsigned row)
		{
			auto gaps = row ? (row - 1) : 0;
			return (charH * row) + (interRowGap * gaps);
		}
	};

	SelectionRenderer(const HaxStringRenderer& renderer):
		m_renderer(renderer)
	{}

	virtual ~SelectionRenderer()
	{}

	/*!
	 * Set the layout parameters for the selection
	 * @param layout
	 */
	void SetLayout(const Layout& layout)
	{
		m_layout = layout;
		// TODO: recompute?
	}

	/*!
	 * Update with a new selection object
	 * @param selection
	 * @return true if anything changed
	 */
	bool UpdateSelection(const HaxDocument::Selection& selection)
	{
		m_sel = selection;
		regenerateSelection();
		return true;
	}

protected:

	// the renderer drives the knowledge of the data layout
	const HaxStringRenderer& m_renderer;
	Layout m_layout; //TODO access via renderer?

	HaxDocument::Selection m_sel;

private:
	/*!
	 * Implement this to turn the selection into the relevant representation
	 */
	virtual void regenerateSelection() = 0;
};

/*!
 * Renders a selection as a set of points
 */
class SelectionPathRenderer: public SelectionRenderer
{
public:
	SelectionPathRenderer(const HaxStringRenderer& renderer):
		SelectionRenderer(renderer)
	{}

	class Point
	{
	public:
		Point(int x, int y):
			x(x), y(y)
		{}

		int x, y;
	};

	typedef std::vector<Point> PointList;
	typedef std::vector<PointList> RegionList;

	const RegionList& GetPaths() const
	{
		return m_regions;
	}

private:

	void regenerateSelection() override
	{
		m_regions.clear();

		const auto start = m_sel.GetStart();
		const auto end = m_sel.GetEnd();

		const auto startRow = m_renderer.GetRowForOffset(start);
		const auto endRow = m_renderer.GetRowForOffset(end);

		// all on one row, this one is easy!
		if (startRow == endRow)
		{
			const auto startCell = m_renderer.GetColForOffset(start, nullptr);
			const auto endCell = m_renderer.GetColForOffset(end, nullptr);

			const auto startX = m_layout.GetXForCell(startCell);
			const auto endX = m_layout.GetXForCell(endCell);

			const auto startY = m_layout.GetYForRow(startRow);
			const auto endY = m_layout.GetYForRow(endRow);

			m_regions.emplace_back();
			const auto pts = m_regions.begin();
			pts->emplace_back(startX, startY);
			pts->emplace_back(endX, endY);
		}
	}

	// the list of points making up the current selection
	RegionList m_regions;
};



#endif /* HXE_WX_SELECTIONRENDERER_H_ */
