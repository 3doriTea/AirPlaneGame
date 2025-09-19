#include "RectDetector.h"
#include <cmath>
#include "Game.h"
#include "ISystem.h"
#include "ColliderCP.h"
#include "WindowContextUtil.h"
#include "CameraResource.h"
#include "CameraSystem.h"
#include "Entity.h"

using namespace mtgb;

mtgb::RectDetector::RectDetector(const RectDetectorConfig& _config)
	: config{ _config }
{
}

mtgb::RectDetector::RectDetector(RectDetectorConfig&& _config)
	: config{ std::move(_config) }
{
}

void mtgb::RectDetector::UpdateDetection()
{
	UpdateDetection(config);
}

void mtgb::RectDetector::UpdateDetection(RectDetectorConfig& _config)
{
	// 基底クラスの detectedTargets_を更新
	detectedTargets_.clear();
	
	Game::System<ColliderCP>().RectContains(
		_config.detectionRect,
		_config.targetTag,
		&detectedTargets_, // 基底クラスのメンバを使用
		_config.windowContext
	);


	CameraHandleInScene hCamera = WinCtxRes::Get<CameraResource>(_config.windowContext).GetHCamera();
	const Transform& cameraTransform = Game::System<CameraSystem>().GetTransform(hCamera);
	
	// 設定に合致しない要素を取り除く
	detectedTargets_.erase(
		std::remove_if(detectedTargets_.begin(), detectedTargets_.end(),
			[&](const ScreenCoordContainsInfo& info)
			{
				Vector3 toTarget = info.worldPos - cameraTransform.GetWorldPosition();
				Vector3 normal = cameraTransform.Forward();
				float distance = DirectX::XMVector3Dot(toTarget, normal).m128_f32[0];

				// 設定した距離より遠いなら除く
				if (std::abs(distance) > _config.maxDistance)
				{
					return true;
				}

				return false;
			}),
		detectedTargets_.end()
	);
}

void mtgb::RectDetector::UpdateAndSetDetection(RectDetectorConfig& _config)
{
	config = _config;
	UpdateDetection();
}

void mtgb::RectDetector::UpdateAndSetDetection(RectDetectorConfig&& _config)
{
	config = std::move(_config);
	UpdateDetection();
}

bool mtgb::RectDetector::HasDetectedTargets() const
{
	return !detectedTargets_.empty();
}

const std::vector<ScreenCoordContainsInfo>& mtgb::RectDetector::GetDetectedTargets() const
{
	return detectedTargets_;
}

void mtgb::RectDetector::ForEach(std::function<void(ScreenCoordContainsInfo&)> _func)
{
	if (!HasDetectedTargets()) return;

	for (auto& target : detectedTargets_)
	{
		_func(target);
	}
}

void mtgb::RectDetector::ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const
{
	if (!HasDetectedTargets()) return;

	for (const auto& target : detectedTargets_)
	{
		_func(target);
	}
}

bool mtgb::RectDetector::IsLineOfSight(const Vector3& _cameraPos, const ScreenCoordContainsInfo& _targetInfo)
{
	Vector3 toTarget = Vector3::Normalize(_targetInfo.worldPos - _cameraPos);
	ColliderCP& colliderCP = Game::System<ColliderCP>();
	float dist = 0.0f;
	for (const auto& other : detectedTargets_)
	{
		// 自分自身とは判定をしない
		if (_targetInfo.entityId == other.entityId) continue;

		// 遮られていたら false
		if (colliderCP.RayCastHit(_targetInfo.worldPos, toTarget, &dist, other.entityId))
		{
			return false;
		}
	}
	return true;
}
