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

	void SearchTargets();
	/// <summary>
	/// <para> ターゲットに向かって射撃を行う </para>
	/// <para> ターゲットがいない場合は正面方向に射撃</para>
	/// </summary>
	/// <returns></returns>
	void FireAtTarget();
	Vector3 GetCurrentTargetPosition() const;
	void DrawUI() const;

	void Initialize(Transform* owner, const Vector2F& screenCenter, float detectionSize);

	/// <summary>
	/// ターゲットがいるか否かを返す
	/// </summary>
	/// <returns>ターゲットがいる場合は真、いない場合は偽</returns>
	bool HasTarget() const;
};
