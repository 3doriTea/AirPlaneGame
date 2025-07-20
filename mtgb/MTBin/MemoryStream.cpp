#include "MemoryStream.h"

mtbin::MemoryStream::MemoryStream(
	mtbin::Byte* _pBuffer,
	const size_t& _bufferSize) :
	BUFFER_SIZE{ _bufferSize },
	pBuffer_{ _pBuffer },
	currentIndex{ 0 }
{
}

mtbin::MemoryStream::~MemoryStream()
{
}

void mtbin::MemoryStream::Seek(SeekPoint _point)
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

void mtbin::MemoryStream::Seek(SeekDir _dir)
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
