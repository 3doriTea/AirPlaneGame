#pragma once
#include <numeric>
#include <random>
#include "TutorialScene/SpeechQueue.h"

/// <summary>
/// <para>シャッフルして読み上げ要素(SPEECH_ELEMENT)を返す </para>
/// <para>要素が末尾に達している際、デフォルトでは自動でシャッフルする </para>
/// </summary>
class ShuffleSpeechQueue
{
public:
	/// <summary>
	/// <para>シャッフル再生用の音声キュー</para>
	/// </summary>
	ShuffleSpeechQueue(SpeechLines&& _lines, std::mt19937 _randomNumberGenerator_ = std::mt19937(std::random_device{}()));
	ShuffleSpeechQueue(const SpeechLines& _lines, std::mt19937 _randomNumberGenerator_ = std::mt19937(std::random_device{}()));
	~ShuffleSpeechQueue() = default;

	/// <summary>
	/// <para>次の音声要素を取得</para>
	/// <para>内部のシャッフル順に従って要素を取り出し、末尾に達していれば false を返す</para>
	/// </summary>
	/// <param name="_speechElement">取得した音声要素の参照。</param>
	/// <returns>次の要素が取得できた場合は true、末尾の場合は false</returns>
	const bool TryGetNext(SPEECH_ELEMENT& _speechElement);

	/// <summary>
	/// <para>現在の再生インデックスを取得</para>
	/// </summary>
	/// <returns>現在のインデックス</returns>
	const int GetCurrentLine() const { return static_cast<int>(index_); }

	/// <summary>
	/// <para>キューが終了しているか</para>
	/// </summary>
	/// <returns>終了している場合は true、そうでなければ false</returns>
	const bool IsFinished() const { return index_ >= static_cast<int>(lines_.size()); }
	
	/// <summary>
	/// <para>内部の再生順をシャッフルして再生位置を先頭に戻す</para>
	/// </summary>
	void Shuffle();

	/// <summary>
	/// 自動でシャッフルをするか否か設定する
	/// </summary>
	/// <param name="_autoShuffle">自動にするなら true、手動なら falseを指定</param>
	void SetAutoShuffle(bool _autoShuffle) { autoShuffle_ = _autoShuffle; }
private:
	
	SpeechLines lines_; //所有する音声行のリスト

	std::vector<std::size_t> order_; //シャッフルした再生順を示すインデックス配列

	std::size_t index_; // 次に取り出すインデックス(order_ 内の位置)

	bool autoShuffle_; // 末尾まで再生し終わった際に自動でシャッフルをするか
	/// <summary>
	/// <para>乱数生成器(Mersenne Twister)</para>
	/// <para>シードはコンストラクタ引数で渡すか、デフォルトで初期化される</para>
	/// </summary>
	std::mt19937 randomNumberGenerator_;
};