
#ifndef HAXEDITOR__H_
#define HAXEDITOR__H_

#include <string>
#include <iostream>

#include <sigc++-2.0/sigc++/signal.h>


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

	const uint8_t* GetDataAt(uint64_t offset) const
	{
		return &m_data[offset];
	}

	uint64_t GetDataLength() const
	{
		return m_len;
	}

	void SetOffset(uint64_t newOffset)
	{
		m_offset = std::min(GetDataLength(), newOffset);

		// and tell everyone who cares about the new offset
		signal_OffsetChanged.emit(newOffset);
	}

	uint64_t GetOffset() const
	{
		return m_offset;
	}

	sigc::signal<void, uint64_t> signal_OffsetChanged;

private:

	std::vector<uint8_t> m_data;
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

	/*!
	 * Implement to allow this renderer to move a caret
	 *
	 * Default disallows movement from this renderer
	 */
	virtual bool ReqMoveCaretDown(int /*steps*/)
	{
		return false;
	}

	virtual bool ReqMoveCaretRight(int /*steps*/)
	{
		return false;
	}

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
	 * Get the cells chars needed to display the data
	 */
	virtual unsigned GetCellChars() const = 0;

	virtual std::string RenderLine(uint64_t offset) const = 0;

	bool ReqMoveCaretDown(int steps) override
	{
		uint64_t newOffset = 0;

		const uint64_t distance = std::abs(steps) * GetWidth();
		const uint64_t currOffset = getDocument().GetOffset();

		if (steps > 0) // down
		{
			newOffset = std::min(getDocument().GetDataLength(),
					currOffset + distance);
		}
		else
		{
			if (-distance > currOffset)
				newOffset = 0;
			else
				newOffset -= distance;
		}

		bool moved = newOffset != currOffset;
		std::cout << "moved" << " " << newOffset << std::endl;
		if (moved)
			getDocument().SetOffset(newOffset);

		return moved;
	}

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

private:

	std::string RenderLine(uint64_t offset) const override
	{
		std::stringstream ss;
		ss << std::hex << std::uppercase << std::setfill('0');

		int lastOffset = std::min(offset + GetWidth(),
				getDocument().GetDataLength());

		for (int x = offset; x < lastOffset; ++x)
		{
			const uint8_t* d = getDocument().GetDataAt(x);
			ss << std::setw(2) << static_cast<unsigned>(*d) << " ";
		}

		return ss.str();
	}

	unsigned GetCellsPerRow() const override
	{
		// TODO multiple formats?
		return GetWidth();
	}

	unsigned GetCellChars() const override
	{
		return 3;
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

		for (int x = offset; x < lastOffset; ++x)
		{
			const uint8_t* d = getDocument().GetDataAt(x);
			addCharToString((char)*d, ss);
		}

		return ss.str();
	}
private:

	void addCharToString(const char c, std::stringstream& ss) const
	{
		if ((c >= 'a' && c <= 'z' ) || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
			ss << c;
		else
			ss << ".";
	}

	unsigned GetCellsPerRow() const override
	{
		return 1;
	}

	unsigned GetCellChars() const override
	{
		return GetWidth();
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
			ss << std::hex << std::uppercase << std::setfill('0');
			ss << std::setw(getAddrWidth()) << static_cast<uint64_t>(offset) << " ";
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
