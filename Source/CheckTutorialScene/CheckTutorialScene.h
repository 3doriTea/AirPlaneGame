#pragma once
#include <mtgb.h>

class CheckTutorialScene : public mtgb::GameScene
{
public:
	CheckTutorialScene();
	~CheckTutorialScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

private:
	TimerHandle hToNextSceneTimer_;  // プレイシーン遷移までのタイマー
	bool isSkipTutorial_;  // チュートリアルをスキップするか
	ImageHandle hBackground_;  // 背景画像
};
