#pragma once
#include <DirectXMath.h>

namespace mtgb
{
	struct Vector2 : DirectX::XMFLOAT2
	{
		Vector2(const float _x, const float _y);
		Vector2() :
			Vector2{ 0, 0 }
		{}

		static inline auto Zero() { return Vector2{ 0, 0 }; }
		static inline auto One() { return Vector2{ 1, 1 }; }
	};
}
