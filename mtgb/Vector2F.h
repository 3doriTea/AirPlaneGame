#pragma once
#include "Vector2T.h"
namespace mtgb
{
	using Vector2F = Vector2T<float>;
	template<>
	inline Vector2T<float> Vector2T<float>::Zero()
	{
		return Vector2T<float>(0.0f, 0.0f);
	}
	template<>
	inline Vector2T<float> Vector2T<float>::One()
	{
		return Vector2T<float>(1.0f, 1.0f);
	}


}