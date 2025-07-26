#include "BinaryWriter.h"

mtnet::BinaryWriter::BinaryWriter(
	mtnet::Byte* _pWriteBuffer,
	const size_t& _bufferSize) :
	MemoryStream{ _pWriteBuffer, _bufferSize }
{
}

mtnet::BinaryWriter::~BinaryWriter()
{
}
