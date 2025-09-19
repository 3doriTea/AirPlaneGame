#pragma once

#include <mtgb.h>
#include "IDetector.h"
#include "RectDetector.h"
#include "CircleDetector.h"
#include "RayDetector.h"
#include <memory>
#include "DetectorConfigs.h"
#include <type_traits>

/// <summary>
/// 敵を自動でロックオンし、射撃をする
/// </summary>
struct TargetingSystem
{
	template<typename DetectorConfigType>
	TargetingSystem(Transform* _owner, const DetectorConfigType& _config);
	~TargetingSystem();

	// ターゲット検出関連
	IDetector* detector;
	ScreenCoordContainsInfo* currentTarget;

	// レティクル描画関連
	float reticleRadius;
	RectF reticleRect;
	ImageHandle targetReticleImage;
	UIParams uiParams;

	// TargetingSystemを所有するオブジェクトのTransform
	Transform* ownerTransform;

	void SearchTargets();
	
	/// <summary>
	/// <para> ターゲットに向かって射撃を行う </para>
	/// <para> ターゲットがいない場合は正面方向に射撃</para>
	/// </summary>
	void FireAtTarget();
	
	Vector3 GetCurrentTargetPosition() const;

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI() const;

	/// <summary>
	/// ターゲットがいるか否かを返す
	/// </summary>
	/// <returns>ターゲットがいる場合は真、いない場合は偽</returns>
	bool HasTarget() const;
};

template<typename DetectorConfigType>
inline TargetingSystem::TargetingSystem(Transform* _owner, const DetectorConfigType& _config)
	: ownerTransform{ _owner }
	, reticleRadius{ 30.0f }
	, reticleRect{}
	, targetReticleImage{ -1 }
	, detector{ nullptr }
	,currentTarget{ nullptr }
{
	if constexpr (std::is_same_v<DetectorConfigType, RectDetectorConfig>)
	{
		detector = new RectDetector(_config);
	}
	else if constexpr (std::is_same_v<DetectorConfigType, CircleDetectorConfig>)
	{
		detector = new CircleDetector(_config);
	}
	else if constexpr (std::is_same_v<DetectorConfigType, RayDetectorConfig>)
	{
		detector = new RayDetector(_config);
	}
	else
	{
		static_assert(false, "有効でない型でした");
	}

	
	// 画像を読み込む
	targetReticleImage = Image::Load("Image/lockOnReticle.png");

	// レティクル矩形のサイズを設定
	reticleRect.size = { reticleRadius * 2.0f, reticleRadius * 2.0f };
}
