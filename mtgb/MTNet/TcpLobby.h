#pragma once
#include "TcpListener.h"
#include "LobbyCommon.h"
#include <thread>
#include <mutex>
#include <list>
#include <map>
#include <queue>


namespace mtnet
{
	/// <summary>
	/// <para>Tcp�ʐM�����郍�r�[</para>
	/// <para>�p�����Ďg��</para>
	/// </summary>
	class TcpLobby
	{
	public:
		/// <summary>
		/// <para>���b�Z�[�W����M�����Ƃ��̃R�[���o�b�N</para>
		/// <para>�����FPlayerId, const BinaryReader*</para>
		/// </summary>
		using OnMessageCallback = std::function<void(
			PlayerId _senderId,
			BinaryReader* _receiveBuffer)>;

	public:
		TcpLobby(
			const IPEndPoint& _local,
			const char* _pCheckCode);
		virtual ~TcpLobby();

		std::string GetLobbyCode() const;

		void Open();
		void Close();

		void SendToBroadcast(const WriteSendBufferCallback& _callback);
		void SendToSession(
			PlayerId _sessionId,
			const WriteSendBufferCallback& _callback);

		/// <summary>
		/// �v���C���[���Q�������Ƃ��̏���
		/// </summary>
		/// <param name="_callback">�R�[���o�b�N�֐�</param>
		void OnJoinedPlayer(const OnJoinedPlayerCallback& _callback);

		void OnMessage(const OnMessageCallback& _callback);

	private:
		/// <summary>
		/// �N���C�A���g�Ƃ�1��1�̃Z�b�V��������
		/// </summary>
		/// <param name="_client">�N���C�A���g</param>
		/// <param name="_sessionId">�Z�b�V�����̎��ʎq</param>
		void StartSession(TcpClient* _client, PlayerId _sessionId);

		/// <summary>
		/// ����̃N���C�A���g�֑��M
		/// </summary>
		/// <param name="_sessionId">���肽���Z�b�V����Id</param>
		/// <param name="_pBuffer">�o�C�g�z��̃A�h���X</param>
		/// <param name="_bufferSize">�o�C�g�z��̃T�C�Y</param>
		void ClientSendQueueAdd(PlayerId _sessionId, mtnet::Byte* _pBuffer, const size_t& _bufferSize);

	private:
		TcpListener tcpListener_;
		OnJoinedPlayerCallback onJoinedPlayerCallback_;  // �v���C���[���Q�����Ă����Ƃ��̃R�[���o�b�N
		OnMessageCallback onMessageCallback_;  // �Q���v���C���[���烁�b�Z�[�W����M�����Ƃ��̃R�[���o�b�N
		std::thread lobbyThread_;  // �󂯕t�����r�[�̔񓯊�����
		std::list<std::thread> clientSessionThreads_;  // �e�N���C�A���g�Ƃ̃Z�b�V�����񓯊�����
		std::map<PlayerId, std::queue<SendQueueElement*>*> clientSendQueue_;  // �e�N���C�A���g�ɑ��M�\��̗v�f����
		std::map<PlayerId, std::mutex*> mutexClientWriteSendQueue_;  // �e�N���C�A���g�̑��M�L���[�̔r������p

		PlayerId playerIdCount_;  // �v���C���[Id�J�E���^

		bool isRunningLobby_;  // ���r�[���ғ�����

		const char* CHECK_CODE;  // �n���h�V�F�C�N���̊m�F�R�[�h

		//static const size_t CLIENT_BUFFER_SIZE;  // �Z�b�V�������̑��E��o�b�t�@�T�C�Y(byte)
	};
}
