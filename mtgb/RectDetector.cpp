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
	Game::System<ColliderCP>().RectContains(
		_config.detectionRect,
		_config.targetName,
		&detectedTargets,
		_config.windowContext
	);

	CameraHandleInScene hCamera = WinCtxRes::Get<CameraResource>(_config.windowContext).GetHCamera();
	const Transform& cameraTransform = Game::System<CameraSystem>().GetTransform(hCamera);
	
	// ê›íËÇ…çáívÇµÇ»Ç¢óvëfÇéÊÇËèúÇ≠
	detectedTargets.erase(
		std::remove_if(detectedTargets.begin(), detectedTargets.end(),
			[&](const RectContainsInfo& info)
			{
				Vector3 toTarget = info.worldPos - cameraTransform.position;
				Vector3 normal = cameraTransform.Forward();
				float distance = DirectX::XMVector3Dot(toTarget, normal).m128_f32[0];

				// ê›íËÇµÇΩãóó£ÇÊÇËâìÇ¢Ç»ÇÁèúÇ≠
				if (std::abs(distance) > _config.maxDistance)
				{
					return true;
				}

				// ÉJÉÅÉâÇ©ÇÁéãê¸Ç™í Ç¡ÇƒÇ¢Ç»Ç¢Ç»ÇÁèúÇ≠
				if (!IsLineOfSight(cameraTransform.position, info))
				{
					return true;
				}

				return false;
				
			}),
			detectedTargets.end()
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
	return !detectedTargets.empty();
}

void mtgb::RectDetector::ForEach(std::function<void(RectContainsInfo&)> _func)
{
	if (HasDetectedTargets()) return;

	for (auto& target : detectedTargets)
	{
		_func(target);
	}
}

void mtgb::RectDetector::ForEach(std::function<void(const RectContainsInfo&)> _func) const
{
	if (!HasDetectedTargets()) return;

	for (const auto& target : detectedTargets)
	{
		_func(target);
	}
}



bool mtgb::RectDetector::IsLineOfSight(const Vector3& _cameraPos, const RectContainsInfo& _targetInfo)
{
	Vector3 toTarget = Vector3::Normalize(_targetInfo.worldPos - _cameraPos);
	ColliderCP& colliderCP = Game::System<ColliderCP>();
	float dist = 0.0f;
	for (const auto& other : detectedTargets)
	{
		// é©ï™é©êgÇ∆ÇÕîªíËÇÇµÇ»Ç¢
		if (_targetInfo.entityId == other.entityId) continue;

		// é’ÇÁÇÍÇƒÇ¢ÇΩÇÁ false
		if (colliderCP.RaycastHit(_targetInfo.worldPos, toTarget, &dist, other.entityId))
		{
			return false;
		}
	}
	return true;
}
