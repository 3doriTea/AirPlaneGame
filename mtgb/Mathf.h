#pragma once
#include <cmath>
#include "MTAssert.h"

namespace mtgb::Mathf
{
	/// <summary>
	/// aからbを線形補間する
	/// </summary>
	/// <param name="_a">a値</param>
	/// <param name="_b">b値</param>
	/// <param name="_value">0.0f ~ 1.0f</param>
	/// <returns>最小値から最大値を線形補間した_valueの値</returns>
	static inline float LerpForward(const float _a, const float _b, const float _value)
	{
		float min{ std::fminf(_a, _b) };
		float max{ std::fmaxf(_a, _b) };
		float rate{ std::fmaxf(0.0f, std::fminf(_value, 1.0f)) };
		
		return min + (max - min) * rate;
	}

	/// <summary>
	/// aからbを線形補間する
	/// </summary>
	/// <param name="_a">a値</param>
	/// <param name="_b">b値</param>
	/// <param name="_normal">0.0f ~ 1.0f</param>
	/// <returns>最小値から最大値を線形補間した_valueの値</returns>
	static inline float Lerp(const float _a, const float _b, const float _value)
	{
		float rate{ std::fmaxf(0.0f, std::fminf(_value, 1.0f)) };

		return _a + (_b - _a) * rate;
	}

	/*static inline float LerpForward(const float _a, const float _b, const float _value)
	{
		float rate{ std::fmaxf(0.0f, std::fminf(_value, 1.0f)) };

		return _b + (_a - _b) * rate;
	}*/

	/// <summary>
	/// 最小値と最大値の範囲で正規化する
	/// </summary>
	/// <param name="_min">最小値</param>
	/// <param name="_max">最大値</param>
	/// <param name="_current">現在値</param>
	/// <returns>正規化された値</returns>
	static inline float Normalize(const float _min, const float _max, const float _current)
	{
		massert(_max - _min > FLT_EPSILON
			&& "最小値、最大値を同じ値にできません。ゼロ除算が発生します。 @Mathf::Normalize");

		return (_current - _min) / (_max - _min);
	}
}
