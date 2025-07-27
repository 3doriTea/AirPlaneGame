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
	/// <para>Tcp通信をするロビー</para>
	/// <para>継承して使う</para>
	/// </summary>
	class TcpLobby
	{
	public:
		/// <summary>
		/// <para>メッセージを受信したときのコールバック</para>
		/// <para>引数：PlayerId, const BinaryReader*</para>
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
		/// プレイヤーが参加したときの処理
		/// </summary>
		/// <param name="_callback">コールバック関数</param>
		void OnJoinedPlayer(const OnJoinedPlayerCallback& _callback);

		void OnMessage(const OnMessageCallback& _callback);

	private:
		/// <summary>
		/// クライアントとの1対1のセッション処理
		/// </summary>
		/// <param name="_client">クライアント</param>
		/// <param name="_sessionId">セッションの識別子</param>
		void StartSession(TcpClient* _client, PlayerId _sessionId);

		/// <summary>
		/// 特定のクライアントへ送信
		/// </summary>
		/// <param name="_sessionId">送りたいセッションId</param>
		/// <param name="_pBuffer">バイト配列のアドレス</param>
		/// <param name="_bufferSize">バイト配列のサイズ</param>
		void ClientSendQueueAdd(PlayerId _sessionId, mtnet::Byte* _pBuffer, const size_t& _bufferSize);

	private:
		TcpListener tcpListener_;
		OnJoinedPlayerCallback onJoinedPlayerCallback_;  // プレイヤーが参加してきたときのコールバック
		OnMessageCallback onMessageCallback_;  // 参加プレイヤーからメッセージを受信したときのコールバック
		std::thread lobbyThread_;  // 受け付けロビーの非同期処理
		std::list<std::thread> clientSessionThreads_;  // 各クライアントとのセッション非同期処理
		std::map<PlayerId, std::queue<SendQueueElement*>*> clientSendQueue_;  // 各クライアントに送信予定の要素たち
		std::map<PlayerId, std::mutex*> mutexClientWriteSendQueue_;  // 各クライアントの送信キューの排他制御用

		PlayerId playerIdCount_;  // プレイヤーIdカウンタ

		bool isRunningLobby_;  // ロビーが稼働中か

		const char* CHECK_CODE;  // ハンドシェイク時の確認コード

		//static const size_t CLIENT_BUFFER_SIZE;  // セッション中の送・受バッファサイズ(byte)
	};
}
