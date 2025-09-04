#pragma once
#include <mtgb.h>
#include "MTStat/MTStat.h"
#include "../TextBox.h"

class TextBoxTimeBar;
class TextBoxBackground;

class TutorialScene : public mtgb::GameScene
{
public:
	enum STATE
	{
		S_HANDSHAKE = 0,  // 挨拶
		S_SHOW_TIMER_NOLMA = 4,  // 時間内にノルマを
		S_LEFT_SIDE = 5,  // 左の君は
		S_RIGHT_SIDE = 6,  // 右の君は
		S_STICK = 7,  // スティックを倒して
		S_TRIGGER = 8,  // トリガーを押すと
		S_SLIDER = 9,  // 左の運転手スライダ
		S_GL = 10,  // 幸運を祈る
	};

public:
	TutorialScene();
	~TutorialScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

	/// <summary>
	/// 現在のステートを取得する
	/// </summary>
	/// <returns>現在のステート</returns>
	const STATE GetState() const { return state_.Current(); }

private:
	TimerHandle hToPlaySceneTimer_;  // プレイシーン遷移までのタイマー
	mtstat::MTStat<STATE> state_;  // ステート管理
	TextBox* pTextBox_;  // 字幕
	float textBoxToChangeTimeLeft_;  // 字幕切り替えまでの残り時間

	TextBoxTimeBar* pTextBoxTimeBar_;
	TextBoxBackground* pTextBoxBackground_;
};
