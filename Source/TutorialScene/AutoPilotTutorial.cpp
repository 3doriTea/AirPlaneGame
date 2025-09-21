#include "AutoPilotTutorial.h"

bool AutoPilotTutorial::TryUpdate()
{
	// ‚à‚µyÀ•W‚ª0–¢–ž‚È‚ç
	if (pTransform_->GetWorldPosition().y < 0.0f)
	{
		// ‘O•ûŒüA“ª‚Íã•ûŒü‚É
		Vector3 direction{ (Vector3::Forward() + Vector3::Up()).Normalize() };
		pTransform_->rotate = Quaternion::SLerp(pTransform_->rotate, Quaternion::LookRotation(direction, Vector3::Up()), 0.01f);

		return true;
	}
	return false;
}
