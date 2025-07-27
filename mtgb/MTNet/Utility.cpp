#include "Utility.h"
#include "LobbyCommon.h"
#include "TcpListener.h"
#include <wincrypt.h>
#include <random>

#pragma comment(lib, "Crypt32.lib")  // wincryptのライブラリの読み込み

std::string mtnet::GetLobbyCode(const TcpListener& _listener)
{
	Octet ipCode[LOBBY_CODE_BYTE_SIZE]{};

	// 頭4byte分でIPアドレスのオクテットを入れる
	if (_listener.LOCAL_IP_END_POINT.TryGetOctets(
		&ipCode[0], &ipCode[1], &ipCode[2], &ipCode[3]) == false)
	{
		throw "IPアドレスからオクテットの取得に失敗";
	}

	// 残り2byteにポート番号を入れる
	PortNumber port{ _listener.LOCAL_IP_END_POINT.GetPortNumber() };
	ipCode[4] = (port >> (8 * 0)) & 0x00ff;
	ipCode[5] = (port >> (8 * 1)) & 0x00ff;

	// base64を書き出すバッファを用意
	char ipCodeBuffer[LOBBY_CODE_STRING_SIZE]{};
	DWORD ipCodeBufferSize{ LOBBY_CODE_STRING_SIZE };

	// いざ、書き出し
	BOOL succeed = CryptBinaryToString(
		ipCode,
		LOBBY_CODE_BYTE_SIZE,
		CRYPT_STRING_BASE64,
		ipCodeBuffer,
		&ipCodeBufferSize);

	if (succeed == FALSE)
	{
		throw "base64への変換に失敗";
	}

	return std::string{ ipCodeBuffer };
}

void mtnet::GenerateChallengeCode(
	char* _out,
	const size_t& _size)
{
	// 乱数とかの設定

	std::random_device seedGenerate{};
	std::mt19937 randomEngine{ seedGenerate() };
	std::uniform_int_distribution<int> distribution{ INT8_MIN, INT8_MAX };

	// 乱数セットしていく
	for (int i = 0; i < _size; i++)
	{
		_out[i] = static_cast<int8_t>(distribution(randomEngine));
	}
}
