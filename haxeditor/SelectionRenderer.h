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

		int GetXForPos(const HaxStringRenderer::StringLinePos& pos, bool left) const
		{
			auto x = charW * pos.chars + interCellGap * pos.gaps;

			// trim off any gaps if we are on the right
			if (!left && pos.gaps)
				x -= interCellGap;

			return x;
		}

		int GetYForRow(const unsigned row, bool top) const
		{
			auto y = (charH + interRowGap) * row;

			// add the current row body if we should be at the bottom of it
			if (!top)
				y += charH;

			return y;

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

		//const auto bpc = m_renderer.GetBitsPerCell();

		// all on one row, this one is easy!
		if (startRow == endRow)
		{
			const auto posS = m_renderer.GetOffsetPosInLine(start);
			const auto posE = m_renderer.GetOffsetPosInLine(end);

			const auto startX = m_layout.GetXForPos(posS, true);
			const auto endX = m_layout.GetXForPos(posE, false);

			const auto startY = m_layout.GetYForRow(startRow, true);
			const auto endY = m_layout.GetYForRow(endRow, false);

			m_regions.emplace_back();
			const auto pts = m_regions.begin();
			pts->emplace_back(startX, startY);
			pts->emplace_back(endX, startY);
			pts->emplace_back(endX, endY);
			pts->emplace_back(startX, endY);
		}
	}

	// the list of points making up the current selection
	RegionList m_regions;
};



#endif /* HXE_WX_SELECTIONRENDERER_H_ */
