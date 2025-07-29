#include "Socket.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

mtnet::Socket::Socket(const IPEndPoint& _local) :
	wsaData_{},
	connectingSocket_{ INVALID_SOCKET },
	result_{ 0 },
	LOCAL_IP_END_POINT{ _local },
	addr_{}
{
}

mtnet::Socket::~Socket()
{
}

bool mtnet::Socket::TryStartup()
{
	// wsaのスタート処理、MAKEWORD -> バージョン
	result_ = WSAStartup(MAKEWORD(2, 2), &wsaData_);
	if (result_ != 0)
	{
		return false;
	}

	// IPEndPointからアドレスを生成する
	if (LOCAL_IP_END_POINT.TryGenerateSockAddrIn(&addr_) == false)
	{
		return false;
	}

	//
	connectingSocket_ = socket(addr_.sin_family, SOCK_STREAM, IPPROTO_TCP);
	if (connectingSocket_ == INVALID_SOCKET)
	{
		return false;
	}

	return true;  // 起動に成功した
}

bool mtnet::Socket::TryBind()
{
	result_ = bind(
		connectingSocket_,
		reinterpret_cast<sockaddr*>(&addr_),
		sizeof(addr_));
	if (result_ == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		Close(true);
		return false;
	}

	return true;
}

bool mtnet::Socket::TryStartListen()
{
	result_ = listen(connectingSocket_, SOMAXCONN);
	if (result_ == SOCKET_ERROR)
	{
		Close(true);
		return false;
	}

	return true;
}

mtnet::Socket* mtnet::Socket::Accept()
{
	Socket* remoteSocket{ new Socket{ LOCAL_IP_END_POINT } };
	
	int addressLength{ sizeof(SOCKADDR_IN) };
	remoteSocket->connectingSocket_ = accept(
		connectingSocket_,
		reinterpret_cast<sockaddr*>(&remoteSocket->addr_),
		&addressLength);

	if (remoteSocket->connectingSocket_ == INVALID_SOCKET)
	{
		int errorCode{ WSAGetLastError() };
		Close(true);
		return nullptr;
	}

	return remoteSocket;
}

bool mtnet::Socket::TryConnect(const IPEndPoint& _remote)
{
	SOCKADDR_IN remoteAddr{};
	if (_remote.TryGenerateSockAddrIn(&remoteAddr) == false)
	{
		return false;
	}

	result_ = connect(
		connectingSocket_,
		reinterpret_cast<sockaddr*>(&remoteAddr),
		sizeof(remoteAddr));
	if (result_ == SOCKET_ERROR)
	{
		Close(true);
		return false;
	}

	return true;
}

mtnet::ReceivedLength mtnet::Socket::Receive(Byte* buffer, const int& bufferLength)
{
	if (connectingSocket_ == INVALID_SOCKET)
	{
		Close(true);
		return false;
	}
	return recv(connectingSocket_, reinterpret_cast<char*>(buffer), bufferLength, 0);
}

bool mtnet::Socket::TrySend(Byte* buffer, const int& bufferLength)
{
	if (connectingSocket_ == INVALID_SOCKET)
	{
		Close(true);
		return false;
	}

	result_ = send(connectingSocket_, reinterpret_cast<char*>(buffer), bufferLength, 0);
	/*if (result_ == SOCKET_ERROR)
	{
		Close(true);
		return false;
	}*/

	return true;
}

void mtnet::Socket::Close(const bool& _force)
{
	if (_force == false)
	{
		shutdown(connectingSocket_, SD_SEND);
	}

	closesocket(connectingSocket_);
	connectingSocket_ = INVALID_SOCKET;
	//WSACleanup();
}
