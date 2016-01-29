/*
 * StringCellRenderer.cpp
 *
 *  Created on: 28 Jan 2016
 *      Author: John Beard
 */

#include "StringCellRenderer.h"

#include <iomanip>
#include <sstream>

//=============================================================================
// Hex Renderer
//=============================================================================

HaxHexRenderer::HaxHexRenderer(HaxDocument& doc):
	StringCellRenderer(doc)
{}

std::string HaxHexRenderer::RenderLine(offset_t offset) const
{
	std::stringstream ss;
	ss << std::hex << std::uppercase << std::setfill('0');

	int lastOffset = std::min(offset + GetWidth(),
			getDocument().GetDataLength());

	auto& stream = getDocument().GetDataStreamAtBit(offset);

	for (int x = offset; x < lastOffset; x += 8)
	{
		char c;
		if (stream.get(c))
		{
			ss << std::setw(2) << static_cast<unsigned>(c & 0xFF) << " ";
		}
	}

	return ss.str();
}

unsigned HaxHexRenderer::GetCellsPerRow() const
{
	// TODO multiple formats?
	return GetWidth() / 8; // one cell per byte = 2 nibbles
}

unsigned HaxHexRenderer::GetCellChars() const
{
	return 2;
}

HaxHexRenderer::CellList HaxHexRenderer::GetRowCells(offset_t /*offset*/) const
{
	CellList cl;

	// all cells are the same length (2 chars)
	cl.insert(cl.begin(), GetCellsPerRow(), GetCellChars());
	return cl;
}

//=============================================================================
// Text Renderer
//=============================================================================

HaxTextRenderer::HaxTextRenderer(HaxDocument& doc):
	StringCellRenderer(doc)
{}

std::string HaxTextRenderer::RenderLine(uint64_t offset) const
{
	std::stringstream ss;

	int lastOffset = std::min(offset + GetWidth(),
			getDocument().GetDataLength());

	auto& stream = getDocument().GetDataStreamAtBit(offset);

	for (int x = offset; x < lastOffset; x += 8)
	{
		char c;
		if (stream.get(c))
		{
			addCharToString(c, ss);
		}
	}

	return ss.str();
}

unsigned HaxTextRenderer::GetCellsPerRow() const
{
	return 1;
}

unsigned HaxTextRenderer::GetCellChars() const
{
	return GetWidth() / BYTE; // one char per byte
}

DataCellRenderer::CellList HaxTextRenderer::GetRowCells(offset_t /*offset*/) const
{
	CellList cl;

	// one cell
	// TODO should be n cells...
	cl.push_back(GetCellChars());
	return cl;
}

void HaxTextRenderer::addCharToString(const char c, std::stringstream& ss) const
{
	if ((c >= 'a' && c <= 'z' ) || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
		ss << c;
	else
		ss << ".";
}

//=============================================================================
// Address Renderer
//=============================================================================

HaxAddressRenderer::HaxAddressRenderer(HaxDocument& doc):
	StringCellRenderer(doc)
{}

std::string HaxAddressRenderer::RenderLine(uint64_t offset) const
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

unsigned HaxAddressRenderer::GetCellsPerRow() const
{
	return 1;
}

unsigned HaxAddressRenderer::GetCellChars() const
{
	return getAddrWidth();
}

DataCellRenderer::CellList HaxAddressRenderer::GetRowCells(offset_t /*offset*/) const
{
	CellList cl;

	//  one cell of the whole width
	cl.push_back(GetCellChars());
	return cl;
}

unsigned HaxAddressRenderer::getAddrWidth() const
{
	// FIXME
	return 6;
}


