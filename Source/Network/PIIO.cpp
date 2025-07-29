#include "PIIO.h"
#include "MTBin/MemoryStream.h"

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
			using mtbin::Byte;
			using SeekDir = mtbin::MemoryStream::SeekDir;
			using mtbin::MemoryStream;

			isRunning_ = true;

			// ‚¢‚´Ú‘±
			client_.Connect(_serverIPEP);

			Byte* pSendBuffer{ new Byte[BUFFER_SIZE]{} };

			MemoryStream sendMS{ pSendBuffer, BUFFER_SIZE };

			auto SendString
			{
				[&, this](const char* _string) -> void
				{
					ZeroMemory(pSendBuffer, BUFFER_SIZE);
					std::memcpy(pSendBuffer, _string, std::strlen(_string));
					client_.Send(pSendBuffer, BUFFER_SIZE);
				}
			};

			while (true)
			{
				{  // ”r‘¼§Œä
					LOCK(isStoppedMutex_);
					if (isStopped_)
					{
						break;
					}
				}

				if (IsEmptySendQueue())  // ‘—M‚·‚é‚à‚Ì‚ª‚È‚É‚à‚È‚¢
				{
					std::this_thread::sleep_for(  // ­‚µ‘Ò‚Â
						std::chrono::milliseconds(EMPTY_SLEEP_TIME_MILLSEC));
				}
				else
				{
					{  // ”r‘¼§Œä
						LOCK(sendQueueMutex_);

						SendString(sendQueue_.back().c_str());

						sendQueue_.pop();  // Dequeue
					}
				}
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
	json data{};
	data["type"] = "ledset";
	data["rate"] = _status;

	SendJson(data);
}

void Network::PIIO::Stop()
{
	if (isRunning_ == false)
	{
		return;  // ˆ—‚ª‘–‚Á‚Ä‚¢‚È‚¢‚È‚ç–³‹
	}

	{  // ”r‘¼§Œä
		LOCK(isStoppedMutex_);
		isStopped_ = true;
	}

	sessionThread_.join();  // ˆ—Š®—¹‚ğ‘Ò‚Â
}

void Network::PIIO::SendJson(const json& _json)
{
	{  // ”r‘¼§Œä
		LOCK(sendQueueMutex_);
		sendQueue_.push(_json.dump());  // Enqueue
	}
}

bool Network::PIIO::IsEmptySendQueue()
{
	LOCK(sendQueueMutex_);
	return sendQueue_.size() <= 0;
}
