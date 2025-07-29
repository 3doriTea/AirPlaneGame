#pragma once
#include "MTNet/TcpClient.h"
#include "../JsonUtility.h"
#include <thread>
#include <mutex>
#include <queue>

namespace Network
{
	class PIIO
	{
	public:
		/// <summary>
		/// LED 状態の種類
		/// </summary>
		enum LED_STATUS : int32_t
		{
			LEDS_OTHER = -1,  // その他
			LEDS_NORMAL = 1,  // ふつう
			LEDS_WARNNING = 2,  // 危険です!
			LEDS_CLEAR = 3,  // クリアおめでとう
		};

		PIIO(const mtnet::IPEndPoint& _localIPEP);
		~PIIO();

		/// <summary>
		/// ラズパイサーバとの接続を開始する
		/// </summary>
		/// <param name="_serverIPEP">サーバのIPエンドポイント</param>
		void Start(const mtnet::IPEndPoint& _serverIPEP);

		/// <summary>
		/// ファン操作を送信する
		/// </summary>
		/// <param name="_rate">回転数割合 0.0f ~ 1.0f</param>
		void SendFan(const float _rate);
		/// <summary>
		/// LED状態操作を送信する
		/// </summary>
		/// <param name="_status">状態</param>
		void SendLED(const LED_STATUS _status);

		/// <summary>
		/// ラズパイサーバとの接続を切断・ストップする
		/// </summary>
		void Stop();
	private:
		/// <summary>
		/// サーバにjsonデータを送信する
		/// </summary>
		/// <param name="_json">jsonデータ</param>
		void SendJson(const json& _json);

		/// <summary>
		/// 排他制御で安全にキューが空か確認する
		/// </summary>
		/// <returns>キューが空である true / false</returns>
		bool IsEmptySendQueue();

	private:
		bool isRunning_;  // 通信が走っているか

		std::thread sessionThread_;  // サーバとの非同期処理
		mtnet::TcpClient client_;  // TCP 通信をする本体
		std::mutex sendQueueMutex_;  // 送信キューへの読み書き排他制御用
		std::queue<std::string> sendQueue_;  // 送信キュー
		bool isStopped_;  // 停止命令がでているかのフラグ
		std::mutex isStoppedMutex_;  // 停止命令フラグの排他制御用
	};
}
