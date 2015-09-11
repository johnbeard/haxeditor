
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
		m_data(1024)
	{
	    for (int i = 0; i < 1024; ++i)
	    {
	        m_data[i] = 3 * i;
	    }
	}

	const uint8_t* GetDataAt(uint64_t offset) const
	{
		return &m_data[offset];
	}
private:

	std::vector<uint8_t> m_data;
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

	std::string RenderLine(uint64_t offset) const
	{
		std::stringstream ss;
		ss << std::hex << std::uppercase << std::setfill('0');

		const int elements = GetWidth();
		for (int x = 0; x < elements; ++x)
		{
			const uint8_t* d = getDocument().GetDataAt(offset);
			ss << std::setw(2) << static_cast<unsigned>(*d) << " ";
			offset += 1;
		}

		return ss.str();
	}

private:
	// the width of the renderer
	unsigned m_width;
};

#endif // HAXEDITOR__H_
