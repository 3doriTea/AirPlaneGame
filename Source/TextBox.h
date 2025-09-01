#pragma once
#include <string>
#include <mtgb.h>
#include "GameObject.h"

/// <summary>
/// 字幕表示のテキストボックス
/// </summary>
class TextBox : public GameObject
{
public:
	TextBox(const float _popTimeSec, const Vector2F _drawPosition, const int _fontSize);
	TextBox(const std::string _initShowText, const float _popTimeSec, const Vector2F _drawPosition, const int _fontSize);
	~TextBox();
	// 1文字あたりの表示秒数
	void SetPopTimeSec(const float _timeSec);
	/// <summary>
	/// テキストをいざ表示する
	/// </summary>
	/// <param name="_text">表示するテキスト</param>
	void Show(const std::u8string& _text);
	/// <summary>
	/// テキストを非表示にする
	/// </summary>
	void Hide();
	/// <summary>
	/// 字幕再生が終了しているか
	/// </summary>
	/// <returns>終了している true / false</returns>
	bool IsFinished() const;
	void Draw() const override;  // 所属するゲームオブジェクトから呼ぶ

private:
	float popTimeSec_;  // 1文字が表示される時間 (秒)
	std::u8string showText_;  // 表示するテキスト
	size_t currentIndex_;     // 現在表示する文字番号
	bool finished_;           // すべての文字が表示しきったか true / false
	TimerHandle hTimer_;  // 1文字表示のタイマーハンドル
	Vector2F drawPosition_;  // 描画座標
	int fontSize_;  // 文字のフォントサイズ

};