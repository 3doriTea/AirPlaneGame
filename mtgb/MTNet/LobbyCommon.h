#pragma once
#include "Common.h"
#include <cstdint>
#include <functional>
#include "BinaryWriter.h"
#include "BinaryReader.h"

namespace mtnet
{
	using Octet = unsigned char;

	using PlayerId = uint8_t;
	using OnJoinedPlayerCallback = std::function<void(const PlayerId)>;

	enum struct ProtocolHeader : uint8_t
	{
		Empty,  // �����
		Chat,  // �`���b�g
		Variable,  // ���̑��ϒ�
	};

	/// <summary>
	/// ���b�Z�[�W�̃x�[�X(�C���^�t�F�[�X=�������z�N���X)
	/// </summary>
	struct IMessageBase
	{
		IMessageBase(ProtocolHeader _protocolHeader) :
			protocolHeader{ _protocolHeader }
		{}
		inline virtual ~IMessageBase() {};

		virtual void To(mtnet::BinaryWriter* _writer) const = 0;
		virtual void From(mtnet::BinaryReader* _reader) = 0;

		ProtocolHeader protocolHeader;
	};

	struct ChatMessage : IMessageBase
	{
		ChatMessage(const PlayerId _playerId, char* _content) :
			IMessageBase{ ProtocolHeader::Chat },
			playerId{ _playerId },
			content{ _content }
		{}

		ChatMessage() :
			ChatMessage{ 0xffui8, nullptr }
		{}

		
		~ChatMessage()
		{
			if (content != nullptr)
			{
				delete[] content;
				content = nullptr;
			}
		}

		inline void SetText(std::string _text)
		{
			if (content != nullptr)
			{
				delete[] content;
				content = nullptr;
			}

			size_t size{ _text.size() + 1 };
			content = new char[size];
			std::memcpy(content, _text.c_str(), size);
		}

		inline void To(mtnet::BinaryWriter* _writer) const override
		{
			_writer->Write(static_cast<uint8_t>(protocolHeader));

			int32_t contentSize{ static_cast<int32_t>(std::strlen(content) + 1) };
			_writer->Write(playerId);
			_writer->Write(contentSize);
			_writer->Write(content, contentSize);
		}
		inline void From(mtnet::BinaryReader* _reader) override
		{
			//assert(static_cast<ProtocolHeader>(_reader->Read<uint8_t>()) == protocolHeader);

			playerId = _reader->Read<PlayerId>();
			int32_t contentSize{ _reader->Read<int32_t>() };
			if (content != nullptr)
			{
				delete[] content;
				content = nullptr;
			}
			content = new char[contentSize];
			_reader->Read(content, contentSize, contentSize);
		}

		inline const char* String() const { return content; }

		PlayerId playerId;
		char* content;
	};

	/// <summary>
	/// �ϒ����̑��̃��b�Z�[�W
	/// </summary>
	struct VariableMessage : IMessageBase
	{
		VariableMessage() :
			IMessageBase{ ProtocolHeader::Variable },
			size{ 0ULL },
			content{ nullptr }
		{}

		size_t size;
		mtnet::Byte* content;
	};

	/// <summary>
	/// ���M�\���Queue�v�f
	/// </summary>
	struct SendQueueElement
	{
		SendQueueElement(
			mtnet::Byte* _pBuffer,
			const size_t& _bufferSize) :
			pBuffer{ _pBuffer },
			bufferSize{ _bufferSize }
		{}

		SendQueueElement() :
			SendQueueElement{ nullptr, 0 }
		{}

		mtnet::Byte* pBuffer;  // �o�C�g�z��̃A�h���X
		size_t bufferSize;  // �o�C�g�z��̃T�C�Y
	};

	/// <summary>
		/// <para>���M�o�b�t�@�ւ̏������݂�������R�[���o�b�N</para>
		/// <para>�����FBinaryWriter* _sendBuffer</para>
		/// </summary>
	using WriteSendBufferCallback = std::function<void(
		BinaryWriter* _sendBuffer)>;

	constexpr size_t LOBBY_CODE_BYTE_SIZE{ 6 };
	constexpr size_t LOBBY_CODE_STRING_SIZE{ 9 };
	constexpr size_t CHALLENGE_CODE_SIZE{ 32 };
	static const char* EMPTY_MESSAGE{ "None" };
}
