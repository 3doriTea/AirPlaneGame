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

	/// <summary>
	/// 覗き具合を設定する
	/// </summary>
	/// <param name="_rate">0.0 ~ 1.0</param>
	void SetPeekRate(const float _rate) { peekRate_ = _rate; }

	const Vector2Int GenPositionPlane() const;
	const Vector2Int GenPositionMountain() const;

private:
	float peekRate_;  // 覗き具合 0.0 ~ 1.0

	TimerHandle hToNextSceneTimer_;  // プレイシーン遷移までのタイマー
	bool isSkipTutorial_;  // チュートリアルをスキップするか
	ImageHandle hImage_[IL_MAX];
	ImageHandle hBackground_;  // 背景画像
	ImageHandle hPlane_;  // 背景飛行機画像
	ImageHandle hMountain_;  // 背景山画像
};
