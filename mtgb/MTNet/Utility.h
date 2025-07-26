#pragma once
#include <string>

namespace mtnet
{
	class TcpListener;

	/// <summary>
	/// ロビーコードを取得する
	/// </summary>
	/// <param name="_listener">TcpListener</param>
	/// <returns>ロビーコード(base64の文字列)</returns>
	std::string GetLobbyCode(const TcpListener& _listener);

	/// <summary>
	/// チャレンジコードを生成する
	/// </summary>
	/// <param name="_out">チャレンジコードのポインタ渡し</param>
	/// <param name="_size">チャレンジコードのバイトサイズ</param>
	void GenerateChallengeCode(char* _out, const size_t& _size);
}
