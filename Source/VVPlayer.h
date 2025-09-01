#pragma once
#include <mtgb.h>
#include <string>
#include <queue>
#include "MTNet/HttpClient.h"
#include <mutex>

/// <summary>
/// VOICEVOXを再生するためのクラス
/// </summary>
class VVPlayer
{
public:
	VVPlayer();
	~VVPlayer();

	/// <summary>
	/// 指定したテキストを読み上げる
	/// </summary>
	/// <param name="_text">読み上げるテキスト(UTF-8)</param>
	void Play(const std::u8string& _text);

private:
	std::queue<std::u8string> playList_;  // 台詞の待機キュー

	mtnet::HttpClient httpClient_;  // VOICEVOXのAPI利用 http通信クライアント

	bool isPlaying_;  // 再生中か否か
	std::mutex playingMutex_;  // 再生中の排他制御用
	std::thread playingThread_;  // 再生の非同期スレッド
};
