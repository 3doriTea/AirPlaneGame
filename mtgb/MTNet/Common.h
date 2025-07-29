#pragma once
#include <string>
#include <sstream>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
#include <cassert>
#include "MTBin/Common.h"

//#pragma comment(lib, "ws2_32.lib")

namespace mtnet
{
	// IPアドレス
	using IPAddress = std::string;
	// ポート番号
	using PortNumber = unsigned short;
	// 結果コード
	using ResultCode = int;
	// 受け取ったバイト数
	using ReceivedLength = int;
}
