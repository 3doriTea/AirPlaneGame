#pragma once
#include "TcpClient.h"
#include "LobbyCommon.h"
#include <thread>
#include <mutex>
#include <queue>

namespace mtnet
{
	class TcpJoiner
	{
	public:
		using OnRecievedCallback = std::function<void(BinaryReader* _receiveBuffer)>;

	public:
		TcpJoiner(const IPEndPoint& _local, const char* _pCheckCode);
		~TcpJoiner();

		void Connect(const IPEndPoint& _remote);
		void Close();

		/// <summary>
		/// ���M����
		/// </summary>
		/// <param name="_callback">�������݂̃R�[���o�b�N</param>
		void Send(const WriteSendBufferCallback& _callback);
		void OnRecieve(const OnRecievedCallback& _callback);

		inline PlayerId GetSessionId() const { return sessionId_; }

	private:
		TcpClient client_;
		OnRecievedCallback onRecievedCallback_;  // ���b�Z�[�W����M�����Ƃ��̃R�[���o�b�N
		std::thread sessionThread_;  // �Z�b�V�����̔񓯊�����
		std::queue<SendQueueElement> sendQueue_;  // ���M�\��̗v�f����
		std::mutex mutexWriteSendQueue_;  // ���M�\��̗v�f�ɏ������ނƂ��̔r������p

		PlayerId sessionId_;  // �Z�b�V����Id (0�Ȃ疳�Z�b�V����)

		bool isRunningJoiner_;  // �Q������

		const char* CHECK_CODE;  // �n���h�V�F�C�N���̊m�F�R�[�h

		//static const size_t CLIENT_BUFFER_SIZE;  // �Z�b�V�������̑��E��o�b�t�@�T�C�Y(byte)
	};
}
