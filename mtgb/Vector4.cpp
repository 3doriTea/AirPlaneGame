#include "Vector4.h"

mtgb::Vector4::Vector4(
	const float _x,
	const float _y,
	const float _z,
	const float _w)
{
	f[0] = _x;
	f[1] = _y;
	f[2] = _z;
	f[3] = _w;
}

mtgb::Vector4::Vector4(const Vector3& _vec3)
{
	f[0] = _vec3.x;
	f[1] = _vec3.y;
	f[2] = _vec3.z;
	f[3] = 0.0f;
}
