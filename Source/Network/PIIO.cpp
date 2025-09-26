#include "PIIO.h"
#include "MTBin/MemoryStream.h"
#include "Debug.h"

#define LOCK(m) std::lock_guard<std::mutex> lock(m)


namespace
{
	static const size_t BUFFER_SIZE{ 1024 };
	static const int EMPTY_SLEEP_TIME_MILLSEC{ 100 };
}

Network::PIIO::PIIO(const mtnet::IPEndPoint& _localIPEP) :
	client_{ _localIPEP },
	isRunning_{ false },
	isStopped_{ false },
	sendQueue_{}
{
}

Network::PIIO::~PIIO()
{
	Stop();
}

void Network::PIIO::Start(const mtnet::IPEndPoint& _serverIPEP)
{
	sessionThread_ = std::thread
	{
		[&, this]() -> void
		{
			try
			{
				using mtbin::Byte;
				using SeekDir = mtbin::MemoryStream::SeekDir;
				using mtbin::MemoryStream;

				isRunning_ = true;

				// いざ接続
				client_.Connect(_serverIPEP);

				Byte* pSendBuffer{ new Byte[BUFFER_SIZE]{} };

				MemoryStream sendMS{ pSendBuffer, BUFFER_SIZE };

				auto SendString
				{
					[&, this](const char* _string) -> void
					{
						ZeroMemory(pSendBuffer, BUFFER_SIZE);
						std::memcpy(pSendBuffer, _string, std::strlen(_string));
						client_.Send(pSendBuffer, static_cast<int>(std::strlen(_string)));
					}
				};

				while (true)
				{
					{  // 排他制御
						LOCK(isStoppedMutex_);
						if (isStopped_)
						{
							break;
						}
					}

					if (IsEmptySendQueue())  // 送信するものがなにもない
					{
						std::this_thread::sleep_for(  // 少し待つ
							std::chrono::milliseconds(EMPTY_SLEEP_TIME_MILLSEC));
					}
					else
					{
						{  // 排他制御
							LOCK(sendQueueMutex_);

							SendString(sendQueue_.back().c_str());

							sendQueue_.pop();  // Dequeue
						}
					}
				}
			}
			catch (const char* message)
			{
				LOGF("%s\n", message);
			}
			isRunning_ = false;
		}
	};
}

void Network::PIIO::SendFan(const float _rate)
{
	json data{};
	data["type"] = "fanset";
	data["rate"] = _rate;

	SendJson(data);
}

void Network::PIIO::SendLED(const LED_STATUS _status)
{
	if (GetLedStatus() == _status)
	{
		return;  // ステータスに変化がないなら回帰
	}

	LOGF("LEDStatus:%d\n", _status);

	json data{};
	data["type"] = "ledset";
	data["stat"] = _status;

	SendJson(data);
}

void Network::PIIO::Stop()
{
	if (isRunning_ == false)
	{
		return;  // 処理が走っていないなら無視
	}

	{  // 排他制御
		LOCK(isStoppedMutex_);
		isStopped_ = true;
	}

	sessionThread_.join();  // 処理完了を待つ
}

void Network::PIIO::SendJson(const json& _json)
{
	{  // 排他制御
		LOCK(sendQueueMutex_);
		sendQueue_.push(_json.dump());  // Enqueue
	}
}

bool Network::PIIO::IsEmptySendQueue()
{
	LOCK(sendQueueMutex_);
	return sendQueue_.size() <= 0;
}
