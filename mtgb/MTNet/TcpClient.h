#pragma once
#include "Socket.h"

namespace mtnet
{
	/// <summary>
	/// TCP通信をするためのクライアント
	/// </summary>
	class TcpClient : public Socket
	{
	public:
		/// <summary>
		/// Tcpクライアントを作成する
		/// </summary>
		/// <param name="_local">接続元のエンドポイント</param>
		TcpClient(const IPEndPoint& _local);
		~TcpClient();

		/// <summary>
		/// 接続する
		/// </summary>
		/// <param name="_remote">接続先のエンドポイント</param>
		void Connect(const IPEndPoint& _remote);

		ReceivedLength Receive(Byte* _buffer, const int& _bufferLength);
		ReceivedLength Send(const Byte* _buffer, const int& _bufferLength);
	};
}
