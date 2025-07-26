#pragma once
#include "MemoryStream.h"

namespace mtnet
{
	/// <summary>
	/// MemoryStreamの書き込み専用版
	/// </summary>
	class BinaryWriter : public MemoryStream
	{
	public:
		BinaryWriter(
			mtnet::Byte* _pWriteBuffer,
			const size_t& _bufferSize);
		~BinaryWriter();

		using MemoryStream::Write;

	private:  // 書き込み以外はプライベートにする
		using MemoryStream::Read;
		using MemoryStream::Seek;
	};
}
