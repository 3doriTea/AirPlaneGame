#pragma once
#include "MemoryStream.h"

namespace mtnet
{
	/// <summary>
	/// MemoryStreamの読み取り専用版
	/// </summary>
	class BinaryReader : public MemoryStream
	{
	public:
		BinaryReader(
			mtnet::Byte* _pReadBuffer,
			const size_t& _bufferSize);
		~BinaryReader();

		using MemoryStream::Read;

	private:  // 読み取り以外はプライベートにする
		using MemoryStream::Write;
		using MemoryStream::Seek;
	};
}
