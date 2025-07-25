#pragma once
#include <string>
#include <mtgb.h>

class TextBox
{
public:
	TextBox();
	~TextBox();
	// 1文字あたりの表示秒数
	void SetTextSpeedSec(const float _sec);
	void Show(const std::string& _text);  // 文字列をいざ表示する
	bool IsFinished();  // 表示し終わっている true / false
	void Draw() const;  // 所属するゲームオブジェクトから呼ぶ

private:
	// 変数名は仮
	float TextSec_;
	std::string testtext_;
	size_t currentIndex_;
	bool finished_;
	TimerHandle hTimer_;
	TimerHandle cTimer_;
};