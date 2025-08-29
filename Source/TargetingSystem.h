#pragma once

#include <mtgb.h>


/// <summary>
/// 敵を自動でロックオンし、射撃をする
/// </summary>
struct TargetingSystem
{
	TargetingSystem();
	~TargetingSystem();
	// ターゲット検出関連
	RectDetector targetDetector;
	RectContainsInfo* currentTarget;

	// レティクル描画関連
	float reticleRadius;
	RectF reticleRect;
	ImageHandle detectionFrameImage;
	ImageHandle targetReticleImage;
	UIParams uiParams;

	// TargetingSystemを所有するオブジェクトのTransform
	Transform* ownerTransform;

	// 機能
	void SearchTargets();
	void FireAtTarget();
	Vector3 GetCurrentTargetPosition() const;
	void DrawUI() const;

	// 初期化
	void Initialize(Transform* owner, const Vector2F& screenCenter, float detectionSize);

	// ターゲットが存在するかチェック
	bool HasTarget() const;
};
