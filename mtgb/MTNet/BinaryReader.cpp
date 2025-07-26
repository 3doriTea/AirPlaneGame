#include "BinaryReader.h"

mtnet::BinaryReader::BinaryReader(
	mtnet::Byte* _pReadBuffer,
	const size_t& _bufferSize) :
	MemoryStream{ _pReadBuffer, _bufferSize }
{
}

mtnet::BinaryReader::~BinaryReader()
{
}
