#include "VVPlayer.h"
#include "MTNet/HttpHeaderBuilder.h"
#include "JsonUtility.h"
#include <thread>

using mtnet::HttpClient;
using mtnet::HttpHeaderBuilder;

namespace
{
	enum SPEAKER
	{
		METAN,     // 四国めたん
		ZUNDAMON,  // ずんだもん
	};

	const char VV_ENGINE_URL[]{ "127.0.0.1:50021" };
}

VVPlayer::VVPlayer() :
	playList_{},
	httpClient_{}
{
	httpClient_.Initialize(VV_ENGINE_URL);
}

VVPlayer::~VVPlayer()
{
}

void VVPlayer::Play(const std::u8string& _text)
{
	{  // 排他制御
		std::lock_guard lock{ playingMutex_ };
		if (isPlaying_)  // 既にプレイ中なら
		{
			// キューに追加する
			playList_.push(_text);
			return;
		}
	}
	
	// 空いているなら非同期処理を走らせる
	std::thread
	{
		[&, _text, this]()
		{
			{  // 排他制御
				std::lock_guard lock{ playingMutex_ };
				isPlaying_ = true;
			}

			// http postリクエストで読み上げるためのデータ生成
			std::string response = httpClient_.Post(
				HttpHeaderBuilder().ContentType("application/json; charset=UTF-8"),
				"audio_query?text=" + HttpClient::ToPercentURI({ _text.begin(), _text.end() })
				+ "&speaker=" + std::to_string(ZUNDAMON));

			// 音声生成に必要な jsonだけ取り出す
			json responseJson = json::parse(response.substr(response.find('{')));
			std::string jsonStr = responseJson.dump(2);

			// 取り出した jsonを送りwavファイルを生成
			httpClient_.PostAndBinaryResponce(
				HttpHeaderBuilder()
				.ContentType("application/json")
				.Accept("audio/wav")
				.ResponseType("stream"),
				"synthesis?speaker=" + std::to_string(ZUNDAMON) + "&enable_interrogative_upspeak=true",
				[&](std::vector<uint8_t>& buffer) -> void
				{
					std::string wavRow{ buffer.begin(), buffer.end() };

					std::istringstream iss{ wavRow };

					std::string wavRowHeader{};
					size_t contentLength{};

					size_t bodyBeginIndex = wavRow.find("RIFF");
					wavRowHeader = wavRow.substr(0, bodyBeginIndex);

					static const char CONTENT_LENGTH_TEXT[]{ "content-length: " };
					size_t begin = wavRowHeader.find(CONTENT_LENGTH_TEXT) + std::strlen(CONTENT_LENGTH_TEXT);
					size_t end = wavRowHeader.find("\r\n", begin);
					contentLength = std::stoull(wavRowHeader.substr(begin, end - begin));

					byte* wavData{ new byte[contentLength]{} };
					memcpy(wavData, wavRow.substr(bodyBeginIndex).data(), contentLength);

					Game::System<Audio>().PlayOneShotBuffer(wavData, contentLength);

					delete[] wavData;
				},
				jsonStr);

			{  // 排他制御
				std::lock_guard lock{ playingMutex_ };
				isPlaying_ = false;
			}

			// もし次の値があるなら続けて再生
			if (!playList_.empty())
			{
				Play(playList_.back());
				playList_.pop();
			}
		}
	}.detach();
}
