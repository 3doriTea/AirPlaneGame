#include "TcpListener.h"
#include "TcpClient.h"

mtnet::TcpListener::TcpListener(const IPEndPoint& _local) :
	Socket{ _local }
{
	if (TryStartup() == false)
	{
		throw "TcpListener‚Ì‹N“®‚ÉŽ¸”s";
	}
	if (TryBind() == false)
	{
		throw "TcpListener‚ÌIPƒoƒCƒ“ƒh‚ÉŽ¸”s";
	}
}

mtnet::TcpListener::~TcpListener()
{
	Close(true);
}

bool mtnet::TcpListener::TryStartListen()
{
	return Socket::TryStartListen();
}

mtnet::TcpClient* mtnet::TcpListener::AcceptTcpClient()
{
	return static_cast<mtnet::TcpClient*>(Accept());
}
