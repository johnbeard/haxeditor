
#ifndef HAXEDITOR__H_
#define HAXEDITOR__H_

#include <string>
#include <iostream>

#include "utils/CompilerSupport.h"

#include <sigc++-2.0/sigc++/signal.h>

#include "dal/DataAbstractionLayer.h"

#include <cassert>

/*!
 * The base class for a haxeditor view of a file
 *
 * Basic file operations and undo stacks are handled in the FAL, this
 * class deals with the interpretation and modification of the data
 */
class HaxEditorBase
{
public:

    bool openFile(const std::string& /*filename*/)
    {
        return false;
    }
};

class HaxDocument
{
public:

	class Selection
	{
	public:

		offset_t GetStart() const
		{
			return m_start;
		}

		/*!
		 * Set the range. Start is inclusive, end is exclusive.
		 *
		 * So [0,1) is just a one-unit selection.
		 */
		void SetRange(offset_t start, offset_t end)
		{
			m_start = start;
			m_end = end;

			// make sure start is less than end
			if (m_start > m_end)
				std::swap(m_start, m_end);

			signal_changed.emit(*this);
		}

		offset_t GetEnd() const
		{
			return m_end;
		}

		offset_t GetSize() const
		{
			return m_end - m_start;
		}

		sigc::signal<void, const Selection&> signal_changed;

	private:
		offset_t m_start = 0;
		offset_t m_end = 0;
	};

	HaxDocument():
		m_offset(0)
	{
		// fill DA in with some fake data until we get the file IO
		// sorted!
		offset_t len = 3003;
		std::vector<uint8_t> data;
		data.resize(len);
	    for (uint64_t i = 0; i < len; ++i)
	    {
	        data[i] = 3 * i;
	    }

	    m_data = std::make_unique<MemoryDataAbstraction>(data.data(), len);

		m_selection.signal_changed.connect(sigc::mem_fun(this,
				&HaxDocument::notifySelectionChanged));
	}

	~HaxDocument()
	{

	}

	/*!
	 * Note: this will return the byte that contains this offset
	 */
	const uint8_t* GetDataAt(offset_t offset) const
	{
		return m_data->GetDataAtOffset(offset);
	}

	offset_t GetDataLength() const
	{
		return m_data->GetCurrentDataLength() * 8;
	}

	void SetOffset(uint64_t newOffset)
	{
		m_offset = std::min(GetDataLength(), newOffset);

		// and tell everyone who cares about the new offset
		signal_OffsetChanged.emit(newOffset);
	}

	offset_t GetOffset() const
	{
		return m_offset;
	}

	/*!
	 * Get the document selection
	 *
	 * This can be used to get the selection for wrapping in a driver.
	 *
	 * Should have driver in the document too?
	 */
	Selection& GetSelection()
	{
		return m_selection;
	}

	/*!
	 * Const selection, good for rendering
	 */
	const Selection& GetSelection() const
	{
		return m_selection;
	}

	mutable sigc::signal<void, offset_t> signal_OffsetChanged;
	mutable sigc::signal<void, const Selection&> signal_SelectionChanged;

private:

	void notifySelectionChanged(const Selection& changedSelection)
	{
		std::cout << "Document get selection change" << std::endl;
		// forward to the document selection listeners
		signal_SelectionChanged.emit(changedSelection);
	}

	std::unique_ptr<DataAbstraction> m_data;

	// location of the caret within the document
	uint64_t m_offset;

	// the current selection in the document
	// future: multiple selections per document
	Selection m_selection;
};

class HaxDataRenderer
{
public:
	HaxDataRenderer(HaxDocument& doc):
		m_doc(doc)
	{
	}

	virtual ~HaxDataRenderer()
	{}

protected:

	HaxDocument& getDocument() const
	{
		return m_doc;
	}

private:
	HaxDocument& m_doc;
};

#include <iomanip>
#include <sstream>

class HaxStringRenderer: public HaxDataRenderer
{
public:
	HaxStringRenderer(HaxDocument& doc):
		HaxDataRenderer(doc),
		m_width(0)
	{}

	/*!
	 * Gets the width in elements (char width depends on the format...)
	 */
	offset_t GetWidth() const
	{
		return m_width;
	}

	void SetWidth(offset_t width)
	{
		m_width = width;
	}

	virtual unsigned GetCellsPerRow() const = 0;

	/*!
	 * Get the chars per cell
	 *
	 * Eg XX XX XX XX is 2 chars
	 *    YYYY YYYY   is 4 chars
	 */
	virtual unsigned GetCellChars() const = 0;

	unsigned GetBitsPerChar() const
	{
		return GetWidth() / (GetCellsPerRow() * GetCellChars());
	}

	unsigned GetBitsPerCell() const
	{
		return GetCellChars() * GetBitsPerChar();
	}

	offset_t GetRowForOffset(offset_t offset, bool leftEdge) const
	{
		offset_t row = 0;

		if (auto w = GetWidth())
		{
			row = offset / w;

			// end is on a row boundary, but we want the left edge
			if (!leftEdge && (offset % w == 0))
				row -= 1;
		}

		return row;
	}

	/*!
	 * Struct used to define position within a string renderer line. This is
	 * made up of some number of "chars" and some number of "gaps". It's up to
	 * the displayer what these mean
	 */
	struct StringLinePos
	{
		unsigned chars = 0;
		unsigned gaps = 0;
	};

	/*!
	 * Get the position of an offset in terms of a StringLinePos
	 *
	 * @param offset the offset to get the pos of
	 * @return
	 */
	StringLinePos GetOffsetPosInLine(offset_t offset, bool leftEdge) const
	{
		StringLinePos pos;
		const auto w = GetWidth();

		if (!w)
			return pos;

		auto offsetInLine = offset % w;

		if (!leftEdge && (offsetInLine == 0))
		{
			// this means we are asking for the offset on a line boundary
			// we were asked for the right edge, which is the end of the
			// first line
			offsetInLine = w;

			// the left edge is still zero
		}

		const auto nChars = offsetInLine / GetBitsPerChar();
		const auto nCells = nChars / GetCellChars();

		pos.chars = nChars;
		pos.gaps = nCells;

		// if we are on a cell boundary, remove the gap if a right edge is wanted
		// and we are not at the start of the line already
		if (offsetInLine % GetBitsPerCell() == 0
				&& !leftEdge
				&& (offsetInLine != 0))
		{
			pos.gaps -= 1;
		}

		return pos;
	}

	/*!
	 * Get the position of the end of a line (this is the same for all lines)
	 * @return the position
	 */
	StringLinePos GetLineEndPos() const
	{
		// special case defer to general case
		return GetOffsetPosInLine(GetWidth(), false);
	}

	StringLinePos GetLineStartPos() const
	{
		// special case defer to general case
		return GetOffsetPosInLine(0, true);
	}

	virtual std::string RenderLine(offset_t offset) const = 0;

	/*!
	 * List of cell lengths within a row
	 * Entries are the lengths of cells
	 */
	typedef std::vector<int> CellList;

	/*!
	 * Get the cells (and their lengths) for a row in the renderer.
	 */
	virtual CellList GetRowCells(offset_t rowOffset) const = 0;

private:

	// the width of the renderer, in bits
	offset_t m_width;
};

class HaxHexRenderer: public HaxStringRenderer
{
public:
	HaxHexRenderer(HaxDocument& doc):
		HaxStringRenderer(doc)
	{}

	std::string RenderLine(uint64_t offset) const override
	{
		std::stringstream ss;
		ss << std::hex << std::uppercase << std::setfill('0');

		int lastOffset = std::min(offset + GetWidth(),
				getDocument().GetDataLength());

		for (int x = offset; x < lastOffset; x += 8)
		{
			const uint8_t* d = getDocument().GetDataAt(x);
			ss << std::setw(2) << static_cast<unsigned>(*d) << " ";
		}

		return ss.str();
	}

	unsigned GetCellsPerRow() const override
	{
		// TODO multiple formats?
		return GetWidth() / 8; // one cell per byte = 2 nibbles
	}

	unsigned GetCellChars() const override
	{
		return 2;
	}

	CellList GetRowCells(offset_t /*offset*/) const override
	{
		CellList cl;

		// all cells are the same length (2 chars)
		cl.insert(cl.begin(), GetCellsPerRow(), GetCellChars());
		return cl;
	}
};

class HaxTextRenderer: public HaxStringRenderer
{
public:
	HaxTextRenderer(HaxDocument& doc):
		HaxStringRenderer(doc)
	{}

	std::string RenderLine(uint64_t offset) const override
	{
		std::stringstream ss;

		int lastOffset = std::min(offset + GetWidth(),
				getDocument().GetDataLength());

		for (int x = offset; x < lastOffset; x += 8)
		{
			const uint8_t* d = getDocument().GetDataAt(x);
			addCharToString((char)*d, ss);
		}

		return ss.str();
	}

	unsigned GetCellsPerRow() const override
	{
		return 1;
	}

	unsigned GetCellChars() const override
	{
		return GetWidth() / BYTE; // one char per byte
	}

	CellList GetRowCells(offset_t /*offset*/) const override
	{
		CellList cl;

		// one cell
		// TODO should be n cells...
		cl.push_back(GetCellChars());
		return cl;
	}

private:

	void addCharToString(const char c, std::stringstream& ss) const
	{
		if ((c >= 'a' && c <= 'z' ) || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
			ss << c;
		else
			ss << ".";
	}
};

class HaxAddressRenderer: public HaxStringRenderer
{
public:
	HaxAddressRenderer(HaxDocument& doc):
		HaxStringRenderer(doc)
	{}

	std::string RenderLine(uint64_t offset) const override
	{
		std::stringstream ss;

		if (offset <= getDocument().GetDataLength())
		{
			const uint64_t byteOffset = offset / 8;
			ss << std::hex << std::uppercase << std::setfill('0');
			ss << std::setw(getAddrWidth()) << static_cast<uint64_t>(byteOffset) << " ";
		}

		return ss.str();
	}

	unsigned GetCellsPerRow() const override
	{
		return 1;
	}

	unsigned GetCellChars() const override
	{
		return getAddrWidth();
	}

	CellList GetRowCells(offset_t /*offset*/) const override
	{
		CellList cl;

		//  one cell of the whole width
		cl.push_back(GetCellChars());
		return cl;
	}

private:

	unsigned getAddrWidth() const
	{
		// FIXME
		return 6;
	}
};


#endif // HAXEDITOR__H_
