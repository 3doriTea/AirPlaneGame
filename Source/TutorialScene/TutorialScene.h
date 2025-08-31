#pragma once
#include <mtgb.h>
#include "MTStat/MTStat.h"

class TutorialScene : public mtgb::GameScene
{
public:
	enum STATE
	{
		S_HANDSHAKE,  // 挨拶
		S_SHOW_TIMER,  // 時間内に敵を
		S_SHOW_NOLMA,  // ノルマを
		S_LEFT_SIDE,  // 左の君は
		S_RIGHT_SIDE,  // 右の君は
		S_STICK,  // スティックを倒して
		S_TRIGGER,  // トリガーを押すと
		S_
	};

public:
	TutorialScene();
	~TutorialScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

	const STATE GetState() const { return state_.Current(); }

private:
	TimerHandle hToPlaySceneTimer_;  // プレイシーン遷移までのタイマー
	mtstat::MTStat<STATE> state_;
};
