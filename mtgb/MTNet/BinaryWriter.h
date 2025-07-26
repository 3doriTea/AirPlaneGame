#pragma once
#include "MemoryStream.h"

namespace mtnet
{
	/// <summary>
	/// MemoryStream�̏������ݐ�p��
	/// </summary>
	class BinaryWriter : public MemoryStream
	{
	public:
		BinaryWriter(
			mtnet::Byte* _pWriteBuffer,
			const size_t& _bufferSize);
		~BinaryWriter();

		using MemoryStream::Write;

	private:  // �������݈ȊO�̓v���C�x�[�g�ɂ���
		using MemoryStream::Read;
		using MemoryStream::Seek;
	};
}
