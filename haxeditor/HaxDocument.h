/*
 * HaxDocument.h
 *
 *  Created on: 28 Jan 2016
 *      Author: John Beard
 */

#ifndef HAXEDITOR_HAXDOCUMENT_H_
#define HAXEDITOR_HAXDOCUMENT_H_

#include "utils/CompilerSupport.h"

#include <sigc++-2.0/sigc++/signal.h>

#include "dal/ChangeApplicator.h"

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
		m_data = std::make_unique<ChangeApplicator>();

		// initialise with a null stream
		m_baseStream = std::make_unique<MockPatternDataStream>(200);
		m_data->SetBaseData(m_baseStream);

		m_selection.signal_changed.connect(sigc::mem_fun(this,
				&HaxDocument::notifySelectionChanged));
	}

	~HaxDocument()
	{

	}

	/*!
	 * Just returns a reference to a stream you can use to get your data
	 *
	 * NOte that streams are inherently BYTE oriented, not BIT oriented!
	 */
	std::istream& GetDataStream()
	{
		return m_data->GetDataStream();
	}

	/*!
	 * Helper for quickly getting hold of a stream set to the byte for a
	 * given bit. You will have to index to the bit as needed!
	 * @param offset the BIT to index
	 * @return the stream, seekg'd to the byte containing that bit
	 */
	std::istream& GetDataStreamAtBit(offset_t offset)
	{
		auto& stream = GetDataStream();
		stream.seekg(offset / BYTE);
		return stream;
	}

	offset_t GetDataLength() const
	{
		return m_data->GetDataLength();
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

	std::unique_ptr<ChangeApplicator> m_data;

	// location of the caret within the document
	uint64_t m_offset;

	// the current selection in the document
	// future: multiple selections per document
	Selection m_selection;

	std::unique_ptr<BaseDataStream> m_baseStream;
};



#endif /* HAXEDITOR_HAXDOCUMENT_H_ */
