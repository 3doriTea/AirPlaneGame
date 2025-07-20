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
