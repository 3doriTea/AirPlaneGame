#pragma once
#include <mtgb.h>

class TutorialScene : public mtgb::GameScene
{
public:
	TutorialScene();
	~TutorialScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

private:
	TimerHandle hToPlaySceneTimer_;  // プレイシーン遷移までのタイマー
};
