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
		/// 送信する
		/// </summary>
		/// <param name="_callback">書き込みのコールバック</param>
		void Send(const WriteSendBufferCallback& _callback);
		void OnRecieve(const OnRecievedCallback& _callback);

		inline PlayerId GetSessionId() const { return sessionId_; }

	private:
		TcpClient client_;
		OnRecievedCallback onRecievedCallback_;  // メッセージを受信したときのコールバック
		std::thread sessionThread_;  // セッションの非同期処理
		std::queue<SendQueueElement> sendQueue_;  // 送信予定の要素たち
		std::mutex mutexWriteSendQueue_;  // 送信予定の要素に書き込むときの排他制御用

		PlayerId sessionId_;  // セッションId (0なら無セッション)

		bool isRunningJoiner_;  // 参加中か

		const char* CHECK_CODE;  // ハンドシェイク時の確認コード

		//static const size_t CLIENT_BUFFER_SIZE;  // セッション中の送・受バッファサイズ(byte)
	};
}
