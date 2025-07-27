#include "BinaryWriter.h"

mtnet::BinaryWriter::BinaryWriter(
	mtbin::Byte* _pWriteBuffer,
	const size_t& _bufferSize) :
	MemoryStream{ _pWriteBuffer, _bufferSize }
{
}

mtnet::BinaryWriter::~BinaryWriter()
{
}
