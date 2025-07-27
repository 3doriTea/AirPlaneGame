#include "TcpJoiner.h"
#include "Log.h"
#include <chrono>

namespace
{
	constexpr size_t CLIENT_BUFFER_SIZE{ 1024 };
	constexpr int EMPTY_SLEEP_TIME_MILLSEC{ 100 };
}

mtnet::TcpJoiner::TcpJoiner(
	const IPEndPoint& _local,
	const char* _pCheckCode) :
	client_{ _local },
	CHECK_CODE{ _pCheckCode },
	isRunningJoiner_{ false },
	sessionId_{ 0 }
{
}

mtnet::TcpJoiner::~TcpJoiner()
{
}

void mtnet::TcpJoiner::Connect(const IPEndPoint& _remote)
{
	using mtnet::Log;

	if (isRunningJoiner_ == true)
	{
		throw "既に参加中";
	}

	client_.Connect(_remote);  // 参加

	isRunningJoiner_ = true;  // 参加中フラグメント

	sessionThread_ = std::thread
	{
		[&, this]()
		{
			using mtnet::Byte;
			using SeekDir = mtnet::MemoryStream::SeekDir;

			Byte* receiveBuffer = new Byte[CLIENT_BUFFER_SIZE];  // 受信用のバッファ
			Byte* sendBuffer = new Byte[CLIENT_BUFFER_SIZE];  // 送信用のバッファ
			mtnet::ReceivedLength receivedLength{};  // 受け取ったバイト数

			MemoryStream sendBufferMS{ sendBuffer, CLIENT_BUFFER_SIZE };
			MemoryStream receiveBufferMS{ receiveBuffer, CLIENT_BUFFER_SIZE };

			// 文字列を送信する
			auto SendString = [&, this](const char* string) -> void
				{
					ZeroMemory(sendBuffer, CLIENT_BUFFER_SIZE);
					std::memcpy(sendBuffer, string, std::strlen(string));
					client_.Send(sendBuffer, CLIENT_BUFFER_SIZE);
				};

			// 受信した文字列の先頭だけ比較する (一致:true / false)
			auto ReceiveComparePrefix = [&, this](const char* string) -> bool
				{
					ZeroMemory(receiveBuffer, CLIENT_BUFFER_SIZE);
					receivedLength = client_.Receive(receiveBuffer, CLIENT_BUFFER_SIZE);

					if (std::strlen(string) > std::strlen(receiveBuffer))
					{
						return false;  // 文字数不一致
					}

					// 文字列を比較 (一致:true / false)
					for (int i = 0; i < std::strlen(string); i++)
					{
						if (string[i] != receiveBuffer[i])
						{
							return false;
						}
					}
					return true;
				};

			// TODO: ここの暗号任意にする
			SendString("Hello.");

			if (ReceiveComparePrefix("urid:") == false)
			{
				Log::Write("サーバーとのハンドシェイク?失敗");
				return;
			}
			sessionId_ = static_cast<PlayerId>(receiveBuffer[5]);

			/*
			* クライアント側のループ
			* Step1. 送る
			* Step2. 受け取る
			* Step3. >>Step1
			*/

			while (isRunningJoiner_)
			{
				if (sendQueue_.size() <= 0)  // 送信するものがない
				{
					client_.Send(
						EMPTY_MESSAGE,
						static_cast<int>(std::strlen(EMPTY_MESSAGE) + 1));  // 鼓動を送信

					std::this_thread::sleep_for(std::chrono::milliseconds(
						EMPTY_SLEEP_TIME_MILLSEC));  // ちょっと待つ
				}
				else  // 送信するものがある
				{
					{  // 排他制御
						std::lock_guard<std::mutex> lock(mutexWriteSendQueue_);

						// 送信する要素
						SendQueueElement& sendElement{ sendQueue_.back() };
						client_.Send(
							sendElement.pBuffer,
							static_cast<int>(sendElement.bufferSize));  // 送信

						delete[] sendElement.pBuffer;  // 動的確保したバッファ解放
						sendQueue_.pop();  // Dequeue
					}
				}

				ZeroMemory(receiveBuffer, CLIENT_BUFFER_SIZE);
				receiveBufferMS.Seek(SeekDir::Head);
				receivedLength = client_.Receive(receiveBuffer, CLIENT_BUFFER_SIZE);

				if (receivedLength <= 0)
				{
					Log::Write("サーバーが切断したため終了中...");
					break;
				}

				if (std::string{ receiveBuffer } == std::string{ EMPTY_MESSAGE })
				{
					// 空メッセージなら無視
				}
				else  // 空メッセージではない
				{
					//Log::Write(receiveBuffer, CLIENT_BUFFER_SIZE);

					receiveBufferMS.Seek(SeekDir::Head);  // 頭から読み取れるようにしてあげる
					onRecievedCallback_(static_cast<BinaryReader*>(&receiveBufferMS));
				}
			}

			client_.Close();

			delete[] receiveBuffer;  // 受信バッファの解放
			delete[] sendBuffer;  // 送信バッファの解放

			Log::Write("クライアント側セッション終了");
		}
	};
}

void mtnet::TcpJoiner::Close()
{
	if (isRunningJoiner_ == true)
	{
		isRunningJoiner_ = false;  // 退出フラグメント
		sessionThread_.join();  // 非同期処理を待つ
	}
}

void mtnet::TcpJoiner::Send(const WriteSendBufferCallback& _callback)
{
	using mtnet::Byte;

	// MEMO: 送信次第解放してくれるよ
	Byte* writeBuffer{ new Byte[CLIENT_BUFFER_SIZE]{} };  // 送る内容バッファ

	MemoryStream ms{ writeBuffer, CLIENT_BUFFER_SIZE };
	_callback(static_cast<BinaryWriter*>(&ms));  // 送信内容を書いてもらう 

	//Log::Write(writeBuffer, CLIENT_BUFFER_SIZE);

	{  // 排他制御
		std::lock_guard<std::mutex> lock(mutexWriteSendQueue_);
		sendQueue_.push({ writeBuffer, CLIENT_BUFFER_SIZE });
	}
}

void mtnet::TcpJoiner::OnRecieve(const OnRecievedCallback& _callback)
{
	onRecievedCallback_ = _callback;
}

//const size_t mtnet::TcpJoiner::CLIENT_BUFFER_SIZE{ 1024 };
