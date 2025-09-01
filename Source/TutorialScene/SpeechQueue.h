#pragma once
#include <map>
#include <string_view>
#include <vector>

// 読み上げ字幕テキスト
using SpeechText = std::u8string_view;
// 読み上げ音声ファイル
using SpeechAudioFile = std::string_view;
// 読み上げ時間 (秒)
using SpeechTimeSec = float;

/// <summary>
/// 読み上げ1文の要素
/// </summary>
struct SPEECH_ELEMENT
{
	SpeechText text_;
	SpeechAudioFile audioFile_;
	SpeechTimeSec time_;
};

/// <summary>
/// 読み上げ台本
/// </summary>
using SpeechLines = std::vector<SPEECH_ELEMENT>;

/// <summary>
/// 読み上げキュー
/// </summary>
class SpeechQueue
{
public:
	SpeechQueue(SpeechLines&& _lines);
	~SpeechQueue();

	/// <summary>
	/// 次の読み上げ要素の参照を取得
	/// </summary>
	/// <returns>読み上げ要素の参照</returns>
	const SPEECH_ELEMENT& GetNext();

private:
	int current_;  // 現在の読み上げたライン
	SpeechLines lines_;  // 読み上げ台本
};
