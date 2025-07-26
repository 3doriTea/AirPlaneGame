#include "MemoryStream.h"

mtnet::MemoryStream::MemoryStream(
	mtnet::Byte* _pBuffer,
	const size_t& _bufferSize) :
	BUFFER_SIZE{ _bufferSize },
	pBuffer_{ _pBuffer },
	currentIndex{ 0 }
{
}

mtnet::MemoryStream::~MemoryStream()
{
}

void mtnet::MemoryStream::Seek(SeekPoint _point)
{
	// îÕàÕêßå‰
	if (_point < 0)
	{
		_point = 0;
	}
	else if (_point > BUFFER_SIZE)
	{
		_point = BUFFER_SIZE;
	}

	currentIndex = _point;
}

void mtnet::MemoryStream::Seek(SeekDir _dir)
{
	switch (_dir)
	{
	case SeekDir::Head:
		currentIndex = 0;
		break;
	default:
		break;
	}
}
