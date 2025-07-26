#pragma once
#include "TcpClient.h"

namespace mtnet
{
	class TcpLobby
	{
	public:
		TcpLobby();
		~TcpLobby();

		void Open();
		void Close();

	private:
	};
}
