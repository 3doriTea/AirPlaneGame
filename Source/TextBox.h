#pragma once
#include <string>
#include <mtgb.h>
#include "GameObject.h"

class TextBox : public GameObject
{
public:
	TextBox(const float _popTimeSec, const Vector2F _drawPosition);
	TextBox(const std::string _initShowText, const float _popTimeSec, const Vector2F _drawPosition);
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
	bool IsFinished();
	void Draw() const override;  // 所属するゲームオブジェクトから呼ぶ

private:
	float popTimeSec_;
	std::u8string showText_;
	size_t currentIndex_;
	bool finished_;
	TimerHandle hTimer_;
	TimerHandle cTimer_;
	Vector2F drawPosition_;
};