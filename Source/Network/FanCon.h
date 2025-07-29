#pragma once
#include "MTNet/TcpClient.h"

namespace Network
{
	class FanCon
	{
	public:
		FanCon();
		~FanCon();

		void Start();

		void Send();

	private:
		mtnet::TcpClient* pTcpClient_;
	};
}
