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
		IL_MESSAGE,     // メッセージテキスト
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
	const RectF GenTextBoxTimer() const;
	const RectF GenTextBoxMsg() const;
	const RectF GenSlideBar(const float _rate = 1.0f) const;

private:
	float peekRate_;  // 覗き具合 0.0 ~ 1.0
	float toMoveTimeLeft_;  // 遷移までの残り時間

	TimerHandle hToNextSceneTimer_;  // プレイシーン遷移までのタイマー
	bool isSkipTutorial_;  // チュートリアルをスキップするか
	ImageHandle hImage_[IL_MAX];  // 画像ハンドルの配列
};
