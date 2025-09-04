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
	/// 試しに次読み上げる要素を取得
	/// </summary>
	/// <param name="_speechElement">読み上げ要素の参照</param>
	/// <returns>次読み上げる要素がある true / false</returns>
	const bool TryGetNext(SPEECH_ELEMENT& _speechElement);

	/// <summary>
	/// 読み上げが終わったか
	/// </summary>
	/// <returns>終わっている true / false</returns>
	const bool IsFinished() const { return current_ >= static_cast<int>(lines_.size()); }

private:
	int current_;  // 現在の読み上げたライン
	SpeechLines lines_;  // 読み上げ台本
};
