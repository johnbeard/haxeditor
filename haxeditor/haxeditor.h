
#ifndef HAXEDITOR__H_
#define HAXEDITOR__H_

#include <string>
#include <iostream>

#include <sigc++-2.0/sigc++/signal.h>

/*!
 * The offset within a file, expressed in bits (not bytes) to allow for
 * bitwise access
 */
typedef uint64_t offset_t;

/*!
 * A transform to apply to a file for viewing (eg 2-s complement, XOR, ROT13,
 * etc)
 *
 * Not all transforms can be reversed (eg a threshold), so inheritors report if
 * the transform can be reversed, and if so, you can edit the file through
 * the transform
 */
class DataTransform
{
public:
    virtual ~DataTransform()
    {}

    virtual bool isReversible() const = 0;
};

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

#include <vector>

class HaxDocument
{
public:

	class Selection
	{
	public:
		Selection():
			m_start(0),
			m_end(0)
		{}

		offset_t GetStart() const
		{
			return m_start;
		}

		offset_t GetEnd() const
		{
			return m_end;
		}

		offset_t GetSize() const
		{
			return m_end - m_start;
		}

	private:
		offset_t m_start;
		offset_t m_end;
	};

	HaxDocument():
		m_len(3003),
		m_offset(0)
	{
		m_data.resize(m_len);
	    for (uint64_t i = 0; i < m_len; ++i)
	    {
	        m_data[i] = 3 * i;
	    }
	}

	~HaxDocument()
	{

	}

	/*!
	 * Note: this will return the byte that contains this offset
	 */
	const uint8_t* GetDataAt(offset_t offset) const
	{
		return &m_data[offset / 8];
	}

	offset_t GetDataLength() const
	{
		return m_len * 8;
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

	sigc::signal<void, offset_t> signal_OffsetChanged;
	sigc::signal<void, const Selection&> signal_SelectionChanged;

private:

	std::vector<uint8_t> m_data;

	// length in bytes
	uint64_t m_len;

	// location of the caret within the document
	uint64_t m_offset;
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
	unsigned GetWidth() const
	{
		return m_width;
	}

	void SetWidth(unsigned width)
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

	offset_t GetRowForOffset(offset_t offset) const
	{
		return offset / GetWidth();
	}

	/*!
	 *
	 * @param offset the offset to find the position of
	 * @return the cell and char in the offsets row
	 */
	unsigned GetColForOffset(offset_t offset, unsigned* nChars) const
	{
		// in bits
		auto col = offset % GetWidth();
		auto nCells = col / GetBitsPerCell();

		if (nChars)
			*nChars = (col % GetBitsPerCell()) / GetBitsPerChar();

		return nCells;
	}

	virtual std::string RenderLine(offset_t offset) const = 0;

private:

	// the width of the renderer
	unsigned m_width;
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
		return GetWidth() / 8; // one char per byte
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

private:
	unsigned getAddrWidth() const
	{
		// FIXME
		return 6;
	}
};


#endif // HAXEDITOR__H_
