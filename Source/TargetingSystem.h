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
/// 検出方式の種類
/// </summary>
enum class DetectionType
{
    Rectangle,  // 矩形検出
    Circle,     // 円形検出
    Ray         // レイ検出
};

/// <summary>
/// 敵を自動でロックオンし、射撃をする
/// </summary>
struct TargetingSystem
{
	TargetingSystem();
	~TargetingSystem();

	// ターゲット検出関連
	//std::unique_ptr<IDetector> detector;
	IDetector* detector;
	DetectionType detectionType;
	ScreenCoordContainsInfo* currentTarget;

	// 各検出器（必要に応じて切り替え）
	RectDetector rectDetector;
	CircleDetector circleDetector;
	RayDetector rayDetector;

	// レティクル描画関連
	float reticleRadius;
	RectF reticleRect;
	ImageHandle detectionFrameImage;
	ImageHandle targetReticleImage;
	UIParams uiParams;

	// TargetingSystemを所有するオブジェクトのTransform
	Transform* ownerTransform;

	/// <summary>
	/// 検出方式を設定
	/// </summary>
	void SetDetectionType(DetectionType type);

	/// <summary>
	/// 矩形検出器として初期化
	/// </summary>
	void InitializeAsRect(Transform* owner, const Vector2F& screenCenter, float detectionSize);

	/// <summary>
	/// 円形検出器として初期化
	/// </summary>
	void InitializeAsCircle(Transform* owner, const Vector2F& screenCenter, float radius);

	/// <summary>
	/// レイ検出器として初期化
	/// </summary>
	void InitializeAsRay(Transform* owner, const Vector3& rayDirection, float maxAngleDegrees, float maxDistance);

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
	/// 下位互換性のための初期化メソッド
	/// </summary>
	//void Initialize(Transform* owner, const Vector2F& screenCenter, float detectionSize);
	template<typename DetectorConfigType>
	void Initialize(Transform* _owner, const DetectorConfigType& _config);

	/// <summary>
	/// ターゲットがいるか否かを返す
	/// </summary>
	/// <returns>ターゲットがいる場合は真、いない場合は偽</returns>
	bool HasTarget() const;
};

template<typename DetectorConfigType>
inline void TargetingSystem::Initialize(Transform* _owner, const DetectorConfigType& _config)
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
}
