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

		/*
		 * Layout of char/gap based data is in a grid with lines like this:
		 *
		 * | char | char |gap| char |gap| char | char |
		 *
		 * Note, chars are not necessarily evenly spaced - it's up to the
		 * renderer to tell you how many chars and gaps there are for a specific
		 * offset in a specific line
		 */

		int charW, charH;
		int interCellGap;
		int interRowGap;

		int GetXForPos(const HaxStringRenderer::StringLinePos& pos) const
		{
			auto x = charW * pos.chars + interCellGap * pos.gaps;
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

		// null selection - there are no paths for this
		if (start == end)
			return;

		const auto startRow = m_renderer.GetRowForOffset(start, true);
		const auto endRow = m_renderer.GetRowForOffset(end, false);

		// all on one row, this one is easy!
		if (startRow == endRow)
		{
			const auto posS = m_renderer.GetOffsetPosInLine(start, true);
			const auto posE = m_renderer.GetOffsetPosInLine(end, false);

			const auto startX = m_layout.GetXForPos(posS);
			const auto endX = m_layout.GetXForPos(posE);

			const auto startY = m_layout.GetYForRow(startRow, true);
			const auto endY = m_layout.GetYForRow(endRow, false);

			m_regions.emplace_back();
			const auto pts = m_regions.begin();
			pts->emplace_back(startX, startY);
			pts->emplace_back(endX, startY);
			pts->emplace_back(endX, endY);
			pts->emplace_back(startX, endY);
		}
		else
		{
			/*
			 * For more than one row, we get something like this:
			 *
			 *                   (t0)                 (t1)
			 *                   +--------------------+
			 *  (m0              |                    |
			 *  +----------------+(t3)                +(t2)
			 *  |                                     |
			 *  +(b0)  (b1)+--------------------------+(m1)
			 *  |          |
			 *  +----------+
			 *  (b3)      (b2)
			 *
			 * Using this approach:
			 *  * Work out the box for the top row (pts t[0:3])
			 *  * Work out the box for the bottom row (pts b[0:3])
			 *  * m[0:1] are then known
			 * * Join the points in order
			 */

			// Top block
			const auto topS = m_renderer.GetOffsetPosInLine(start, true);
			const auto topE = m_renderer.GetLineEndPos();

			const auto t0X = m_layout.GetXForPos(topS);
			const auto t1X = m_layout.GetXForPos(topE);

			const auto t0Y = m_layout.GetYForRow(startRow, true);
			const auto t2Y = m_layout.GetYForRow(startRow, false);

			// bottom block
			const auto btmS = m_renderer.GetLineStartPos();
			const auto btmE = m_renderer.GetOffsetPosInLine(end, false);

			const auto b0X = m_layout.GetXForPos(btmS);
			const auto b1X = m_layout.GetXForPos(btmE);

			const auto b0Y = m_layout.GetYForRow(endRow, true);
			const auto b2Y = m_layout.GetYForRow(endRow, false);

			m_regions.emplace_back();
			const auto pts = m_regions.begin();

			pts->emplace_back(t0X, t0Y); // t0
			pts->emplace_back(t1X, t0Y); // t1
			pts->emplace_back(t1X, b0Y); // m1
			pts->emplace_back(b1X, b0Y); // b1
			pts->emplace_back(b1X, b2Y); // b2
			pts->emplace_back(b0X, b2Y); // b3
			pts->emplace_back(b0X, t2Y); // m0
			pts->emplace_back(t0X, t2Y); // t3
		}
	}

	// the list of points making up the current selection
	RegionList m_regions;
};



#endif /* HXE_WX_SELECTIONRENDERER_H_ */
