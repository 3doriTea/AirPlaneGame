#pragma once
#include "MTNet/TcpClient.h"
#include "../JsonUtility.h"

namespace Network
{
	class PIIO
	{
	public:
		PIIO();
		~PIIO();

		void Start();

		void SendFan(const float _rate);
		void SendLED(const int _status);

		void Stop();
	private:
		void SendJson(const json& _json);

	private:
		mtnet::TcpClient* pTcpClient_;
	};
}
