#pragma once
#include <string>
#include <winsock2.h>
#include <functional>
#include <vector>
#include "HttpHeaderBuilder.h"

#pragma comment(lib,"ws2_32.lib")

namespace mtnet
{
	class HttpClient
	{
	public:
		using ResponceCallback = std::function<void(const std::string&)>;
		using BinaryResponceCallback = std::function<void(std::vector<uint8_t>&)>;

	public:
		HttpClient();
		~HttpClient();

		/// <summary>
		/// <para>接続先のURLを指定して初期化</para>
		/// <para>接続先を変更するたびに呼び出す</para>
		/// </summary>
		/// <param name="_remoteUrl">接続先のURL (X.X.X.X:Port)</param>
		void Initialize(const std::string& _remoteUrl);

		/// <summary>
		/// ポストリクエストを送信する
		/// </summary>
		/// <param name="_header">ヘッダ</param>
		/// <param name="_pathAndQuery">http://*.*/{この部分}</param>
		/// <param name="_body">本体(任意)</param>
		void Post(
			HttpHeaderBuilder& _header,
			const std::string& _pathAndQuery,
			const ResponceCallback& _onResponce,
			const std::string& _body = "");

		std::string Post(
			HttpHeaderBuilder& _header,
			const std::string& _pathAndQuery,
			const std::string& _body = "");

		void PostAndBinaryResponce(
			HttpHeaderBuilder& _header,
			const std::string& _pathAndQuery,
			const BinaryResponceCallback& _onRecponce,
			const std::string& _body = "");

		/// <summary>
		/// WSAのクリーンアップ (アプリケーションを閉じるときに呼ぶ)
		/// </summary>
		void Cleanup();

	public:
		/// <summary>
		/// <para>文字列をパーセントURIに変換する</para>
		/// <para>URI内で日本語などは %文字コード になる</para>
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static std::string ToPercentURI(const std::string& str);

	private:
		/// <summary>
		/// 試しにURLをIPアドレスとポート番号に分解する
		/// </summary>
		/// <param name="_srcUrl">元のURL</param>
		/// <param name="_pOctet1">ポインタ渡し:IPアドレスの1オクテット目</param>
		/// <param name="_pOctet2">ポインタ渡し:IPアドレスの2オクテット目</param>
		/// <param name="_pOctet3">ポインタ渡し:IPアドレスの3オクテット目</param>
		/// <param name="_pOctet4">ポインタ渡し:IPアドレスの4オクテット目</param>
		/// <param name="_pPortNumber">ポインタ渡し:ポート番号</param>
		/// <returns>成功した true / false</returns>
		static bool TryToIPAddresAndPort(
			const std::string& _srcUrl,
			BYTE* _pOctet1,
			BYTE* _pOctet2,
			BYTE* _pOctet3,
			BYTE* _pOctet4,
			USHORT* _pPortNumber);

	private:
		WSADATA wsaData_;  // WinSock2でネットワークを使うための
		SOCKET socket_;  // 通信するソケット
		SOCKADDR_IN sockAddr_;  // 通信先のエンドポイント情報
		std::string remoteUri_;  // 接続先のURI
	};
}
