#include "BinaryReader.h"

mtnet::BinaryReader::BinaryReader(
	mtbin::Byte* _pReadBuffer,
	const size_t& _bufferSize) :
	MemoryStream{ _pReadBuffer, _bufferSize }
{
}

mtnet::BinaryReader::~BinaryReader()
{
}
