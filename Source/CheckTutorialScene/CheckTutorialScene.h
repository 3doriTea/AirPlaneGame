#pragma once
#include <mtgb.h>

class CheckTutorialScene : public mtgb::GameScene
{
private:
	enum IMAGE_LAYER
	{
		IL_BACKGROUND,  // 背景
		IL_PLANE,       // 飛行機
		IL_MOUNTAIN,    // 手前の山
		IL_MAX,  // 最大値
	};

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
	ImageHandle hImage_[IL_MAX];
	ImageHandle hBackground_;  // 背景画像
	ImageHandle hPlane_;  // 背景飛行機画像
	ImageHandle hMountain_;  // 背景山画像
};
