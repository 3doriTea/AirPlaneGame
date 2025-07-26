#pragma once
#include "MemoryStream.h"

namespace mtnet
{
	/// <summary>
	/// MemoryStream�̓ǂݎ���p��
	/// </summary>
	class BinaryReader : public MemoryStream
	{
	public:
		BinaryReader(
			mtnet::Byte* _pReadBuffer,
			const size_t& _bufferSize);
		~BinaryReader();

		using MemoryStream::Read;

	private:  // �ǂݎ��ȊO�̓v���C�x�[�g�ɂ���
		using MemoryStream::Write;
		using MemoryStream::Seek;
	};
}
