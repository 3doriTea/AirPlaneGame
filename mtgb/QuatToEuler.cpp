#include "QuatToEuler.h"
#include <algorithm>

mtgb::Vector3 mtgb::QuatToEuler(DirectX::XMVECTORF32 _q)
{
	//分母、分子
	float denom, num;
	float roll, pitch, yaw;
	float x = _q[0], y = _q[1], z = _q[2], w = _q[3];

	//ピッチ(x軸)

	//90～^90の範囲
	float sinX = 2.0f * (w * y - z * x);
	pitch = std::asinf(std::clamp(sinX, -1.0f, 1.0f));

	//ヨー(y軸)
	num = 2.0f * (w * z + x * y);
	denom = 1 - 2.0f * (y * y + z * z);
	yaw = std::atan2f(num, denom);

	//ロール(z軸)
	num = 2.0f * (w * x + y * z);
	denom = 1 - 2.0f * (x * x + y * y);
	roll = std::atan2(num, denom);

	float pitch_deg = DirectX::XMConvertToDegrees(pitch);
	float yaw_deg = DirectX::XMConvertToDegrees(yaw);
	float roll_deg = DirectX::XMConvertToDegrees(roll);

	return { pitch_deg,yaw_deg,roll_deg };
}