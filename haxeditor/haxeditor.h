
#ifndef HAXEDITOR__H_
#define HAXEDITOR__H_

#include <string>


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

    bool openFile(const std::string& filename)
    {
        return false;
    }
};

#include <vector>

class HaxDocument
{
public:

	HaxDocument():
		m_len(303)
	{
		m_data.resize(m_len);
	    for (uint64_t i = 0; i < m_len; ++i)
	    {
	        m_data[i] = 3 * i;
	    }
	}

	const uint8_t* GetDataAt(uint64_t offset) const
	{
		return &m_data[offset];
	}

	uint64_t GetDataLength() const
	{
		return m_len;
	}
private:

	std::vector<uint8_t> m_data;
	uint64_t m_len;
};

class HaxDataRenderer
{
public:
	HaxDataRenderer(const HaxDocument& doc):
		m_doc(doc)
	{}

	virtual ~HaxDataRenderer()
	{}

protected:
	const HaxDocument& getDocument() const
	{
		return m_doc;
	}

private:
	const HaxDocument& m_doc;
};

#include <iomanip>
#include <sstream>

class HaxStringRenderer: public HaxDataRenderer
{
public:
	HaxStringRenderer(const HaxDocument& doc):
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

	virtual std::string RenderLine(uint64_t offset) const = 0;

private:
	// the width of the renderer
	unsigned m_width;
};

class HaxHexRenderer: public HaxStringRenderer
{
public:
	HaxHexRenderer(const HaxDocument& doc):
		HaxStringRenderer(doc)
	{}

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
};

class HaxTextRenderer: public HaxStringRenderer
{
public:
	HaxTextRenderer(const HaxDocument& doc):
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
};

class HaxAddressRenderer: public HaxStringRenderer
{
public:
	HaxAddressRenderer(const HaxDocument& doc):
		HaxStringRenderer(doc)
	{}

	std::string RenderLine(uint64_t offset) const override
	{
		std::stringstream ss;

		if (offset <= getDocument().GetDataLength())
		{
			ss << std::hex << std::uppercase << std::setfill('0');
			ss << std::setw(8) << static_cast<uint64_t>(offset) << " ";
		}

		return ss.str();
	}
};


#endif // HAXEDITOR__H_
