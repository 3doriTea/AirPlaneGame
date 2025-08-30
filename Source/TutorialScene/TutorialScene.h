#pragma once
#include <mtgb.h>
#include "MTStat/MTStat.h"

class TutorialScene : public mtgb::GameScene
{
public:
	enum STATE
	{
		S_
	};

public:
	TutorialScene();
	~TutorialScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

	const STATE GetState() const;

private:
	STATE currentState_;  // 現在のチュートリアルステート
	TimerHandle hToPlaySceneTimer_;  // プレイシーン遷移までのタイマー
};
