#include "QuatToEuler.h"
#include <algorithm>

mtgb::Vector3 mtgb::QuatToEuler(DirectX::XMVECTORF32 _q)
{
	using namespace DirectX;
	//分母、分子
	float denom, num;
	float roll, pitch, yaw;
	float x = _q[0], y = _q[1], z = _q[2], w = _q[3];

	//ピッチ(x軸)

	//90～^90の範囲
	float e = 0.9999f;
	float sinX = -2.0f * (z * x - w * y );
	bool gimbalLock = false;
	if (fabsf(sinX) >= e)
	{
		gimbalLock = true;
	}

	pitch = std::asinf(std::clamp(sinX,-e,e));



	//ヨー(y軸)
	if (gimbalLock)
	{
		num = 2.0f * (w * z + x * y);
		denom = 1 - 2.0f * (w * w + z * z);
	}
	else
	{
		num = 2.0f * (w * z - x * y);
		denom = 1 - 2.0f * (w * w + y * y);
	}
	yaw = std::atan2f(num, denom);

	//ロール(z軸)

	if (gimbalLock)
	{
		num = 2.0f * (w * x + y * z);
		denom = 1 - 2.0f * (w * w + z * z);
		roll = std::atan2f(num, denom);
	}
	else
	{
		roll = 0;
	}

	float pitch_deg = DirectX::XMConvertToDegrees(pitch);
	float yaw_deg = DirectX::XMConvertToDegrees(yaw);
	float roll_deg = DirectX::XMConvertToDegrees(roll);

	return { pitch_deg,yaw_deg,roll_deg };
}