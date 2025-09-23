#include "AutoPilotPlay.h"

namespace
{
	// 作戦範囲外 y軸
	const float DEAD_ZONE_Y{ 20.0f };
	// 範囲外から出たときの向かうべき場所
	const float RETURN_SAFE_Y{ 25.0f };
	const float AUTO_ROTATE_ZONE_Y{ 25.0f };
	const float LOOK_AT_DISTANCE{ 20.0f };

	// スラープの割合定数
	const float LARP_RATE{ 0.01f };

	const float FLIGHT_STICK_DEAD_ZONE{ 0.1f };
}

bool AutoPilotPlay::TryUpdate()
{
	Vector3 pos{ pTransform_->GetWorldPosition() };

	if (pos.y < 1.0f)
	{
		pTransform_->position.y = 10.0f;
	}

	if (pos.y <= DEAD_ZONE_Y)
	{
		Vector3 lookAt{ pTransform_->Forward() * LOOK_AT_DISTANCE + pos };
		lookAt.y = RETURN_SAFE_Y;
		pTransform_->rotate = Quaternion::SLerp(pTransform_->rotate, Quaternion::LookRotation(lookAt - pos, Vector3::Up()), LARP_RATE);
		return true;
	}

	Vector2F axis = InputUtil::GetAxis(WindowContext::First);
	float axisSize{ axis.x * axis.x + axis.y * axis.y };
	if (pos.y <= AUTO_ROTATE_ZONE_Y && axisSize <= FLIGHT_STICK_DEAD_ZONE)
	{
		Vector3 lookAt{ pTransform_->Forward() + pos };
		lookAt.y = pos.y;
		pTransform_->rotate = Quaternion::SLerp(pTransform_->rotate, Quaternion::LookRotation(lookAt - pos, Vector3::Up()), LARP_RATE);
	}

	return false;  // 操作なし
}
