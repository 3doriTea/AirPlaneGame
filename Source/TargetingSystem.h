#pragma once

#include <mtgb.h>
#include "IDetector.h"
#include "RectDetector.h"
#include "CircleDetector.h"
#include "RayDetector.h"
#include <memory>
#include "DetectorConfigs.h"
#include <type_traits>
#include "ThreatData.h"

/// <summary>
/// 敵を自動でロックオンし、射撃をする
/// </summary>
struct TargetingSystem
{
	template<typename DetectorConfigType>
	TargetingSystem(Transform* _owner, const DetectorConfigType& _config);
	~TargetingSystem();

	ThreatLevel currentThreatLevel_;
	EventHandlerId threatEventHandlerId_;
	// ターゲット検出関連
	IDetector* detector;
	ScreenCoordContainsInfo* currentTarget;

	// 検出範囲描画関連
	ImageHandle alertDetectionFrameImage_;
	ImageHandle normalDetectionFrameImage_;


	// レティクル描画関連
	float reticleRadius;
	RectF reticleRect;
	ImageHandle targetReticleImage;
	UIParams uiParams;

	// TargetingSystemを所有するオブジェクトのTransform
	Transform* ownerTransform;

	/// <summary>
	/// 脅威度に応じた画像を返す
	/// </summary>
	/// <returns></returns>
	ImageHandle GetDetectionFrameImage() const;
	void SearchTargets();

	void ClearTarget();
	/// <summary>
	/// <para> ターゲットに向かって射撃を行う </para>
	/// <para> ターゲットがいない場合は正面方向に射撃</para>
	/// </summary>
	void FireAtTarget();
	

	Vector3 GetCurrentTargetPosition() const;

	void OnThreatLevelChanged(const ThreatEventData& _data);
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

template <typename T>
constexpr bool false_v = false;

template<typename DetectorConfigType>
TargetingSystem::TargetingSystem(Transform* _owner, const DetectorConfigType& _config)
	: ownerTransform{ _owner }
	, reticleRadius{ 30.0f }
	, reticleRect{}
	, targetReticleImage{ -1 }
	, detector{ nullptr }
	, currentTarget{ nullptr }
	, currentThreatLevel_{ThreatLevel::Normal}
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
		static_assert(false_v<DetectorConfigBase>, "有効でない型でした");
	}

	uiParams.layerFlag = _config.base.uiParams.layerFlag;

	// 脅威度の変更時に呼ばれるコールバック
	threatEventHandlerId_ = Game::System<EventManager>().GetEvent<ThreatEventData>().Subscribe(
		[this](const ThreatEventData& _data)
		{
			OnThreatLevelChanged(_data);
	});
	
	
	// 画像を読み込む
	targetReticleImage = Image::Load("Image/lockOnReticle.png");
	normalDetectionFrameImage_ = Image::Load("Image/lockOnCircleGreen.png");
	alertDetectionFrameImage_ = Image::Load("Image/lockOnCircleRed.png");

	// レティクル矩形のサイズを設定
	reticleRect.size = { reticleRadius * 2.0f, reticleRadius * 2.0f };
}


