#include "LockOn.h"
#include <algorithm>
#include "Draw.h"
void mtgb::LockOn::Search()
{
	rectDetector.UpdateDetection();

	// ƒ[ƒ‹ƒhÀ•WŒn‚Åˆê”Ô‹ß‚¢“G‚ð‘_‚¤

	auto it = std::min_element(
		rectDetector.detectedTargets.begin(),
		rectDetector.detectedTargets.end(),
		[this](const RectContainsInfo& a, const RectContainsInfo& b)
		{
			float da = (pTransform->position - a.worldPos).Size();
			float db = (pTransform->position - b.worldPos).Size();
			return da < db;
		}
	);

	if (it != rectDetector.detectedTargets.end())
	{
		pTargetInfo = &(*it); // ƒAƒhƒŒƒX‚ð‘ã“ü
		reticleRect.x = pTargetInfo->screenPos.x - reticleSideExtents;
		reticleRect.y = pTargetInfo->screenPos.y - reticleSideExtents;
	}
	else
	{
		pTargetInfo = nullptr; // Œ©‚Â‚©‚ç‚È‚©‚Á‚½ê‡‚Í nullptr
	}
}

mtgb::Vector3 mtgb::LockOn::GetTargetPos()
{
	return pTargetInfo->worldPos;
}

void mtgb::LockOn::Draw() const
{
	//Ž©“®‚Å‘_‚¢‚ð’è‚ß‚é”ÍˆÍ‚ð•`‰æ
	const Vector2Int DRAW_SIZE{ lockOnSide ,lockOnSide };
	Draw::Image(frameImage, rectDetector.config.detectionRect, uiParams);

	//‘_‚¢‚ª’è‚Ü‚Á‚Ä‚¢‚é“G‚ð‹­’²•\Ž¦
	if (rectDetector.HasDetectedTargets())
	{
		//Draw::Image(lockOnReticle_, lockOn_->reticleRect, lockOn_->uiParams);
	}
}
