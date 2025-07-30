#pragma once
#include "Common.h"
#include "EndPoint.h"

namespace mtnet
{
	using mtbin::Byte;

	class Socket
	{
	public:
		Socket(const IPEndPoint& _local);
		~Socket();

		/// <summary>
		/// ソケットを閉じる
		/// </summary>
		/// <param name="force">強制的に閉じる true / false</param>
		void Close(const bool& force = false);

	protected:
		/// <summary>
		/// 試しに起動する
		/// </summary>
		/// <returns>起動できた true / false</returns>
		bool TryStartup();

		/// <summary>
		/// 試しにバインドする
		/// </summary>
		/// <returns>成功した true / false</returns>
		bool TryBind();

		/// <summary>
		/// 試しにTCP受け入れを開始する
		/// </summary>
		/// <returns>成功した true / false</returns>
		bool TryStartListen();

		/// <summary>
		/// リモートソケットを受け入れる
		/// </summary>
		/// <returns>受け入れたソケット</returns>
		Socket* Accept();

		/// <summary>
		/// 試しに接続する
		/// </summary>
		/// <param name="_remote">外部のエンドポイント</param>
		/// <returns>接続できた true / false</returns>
		bool TryConnect(const IPEndPoint& _remote);

		/// <summary>
		/// 受け取る
		/// </summary>
		/// <param name="buffer">受信バッファ</param>
		/// <param name="bufferLength">バッファの長さ</param>
		/// <returns>実際に使ったバッファの長さ</returns>
		mtnet::ReceivedLength Receive(Byte* buffer, const int& bufferLength);

		/// <summary>
		/// 試しに送信する
		/// </summary>
		/// <param name="buffer">送信バッファ</param>
		/// <param name="bufferLength">バッファの長さ</param>
		/// <returns>送信できた true / false</returns>
		bool TrySend(Byte* buffer, const int& bufferLength);

		/// <summary>
		/// 現在の結果コードを取得する
		/// </summary>
		/// <returns>結果コード</returns>
		inline ResultCode GetResultCode() const { return result_; }

	public:
		const IPEndPoint LOCAL_IP_END_POINT;

	protected:
		ResultCode result_;

	private:
		WSADATA wsaData_;
		SOCKADDR_IN addr_;
		SOCKET connectingSocket_;
	};
}
