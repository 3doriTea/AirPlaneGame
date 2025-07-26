#pragma once
#include "Socket.h"

namespace mtnet
{
	class TcpClient;

	/// <summary>
	/// TCP通信をするリスナー (TCPClient同士を通信させる)
	/// </summary>
	class TcpListener : public Socket
	{
	public:
		TcpListener(const IPEndPoint& _local);
		~TcpListener();

		bool TryStartListen();

		TcpClient* AcceptTcpClient();

	private:
	};
}
