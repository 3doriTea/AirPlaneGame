#pragma once
#include <cmath>
#include "MTAssert.h"
#include "Vector3.h"
#include "GameTime.h"

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

	/// <summary>
	/// 正数の中で最小値を求める
	/// </summary>
	/// <param name="_a">値a</param>
	/// <param name="_b">値b</param>
	/// <returns>正数の中での最小値 / 0</returns>
	static float PlusMin(float _a, float _b)
	{
		if (_a < 0 && _b < 0)
		{
			return 0;
		}
		if (_a < 0)
		{
			return _b;
		}
		if (_a < _b)
		{
			return _a;
		}
		else
		{
			return _b;
		}
	}

	/// <summary>
	/// 射撃予測
	/// </summary>
	/// <param name="_shotPosition">撃つ座標</param>
	/// <param name="_targetPosition">ターゲットの座標</param>
	/// <param name="_targetMove">ターゲットの移動(/s)</param>
	/// <param name="_bulletSpeed">弾の速度</param>
	/// <returns>撃って当たる座標</returns>
	static Vector3 TargetingPosition(Vector3 _shotPosition, Vector3 _targetPosition, Vector3 _targetMove, float _bulletSpeed)
	{
		const float BULLET_SPEED{ _bulletSpeed * Time::DeltaTimeF() };
		
		const Vector3 MOVE{ _targetMove * Time::DeltaTimeF() };

		Vector3 toTargetDiff{ _targetPosition - _shotPosition };

		const float A
		{
			(MOVE.x * MOVE.x) + (MOVE.y * MOVE.y) + (MOVE.z * MOVE.z)
			- (BULLET_SPEED * BULLET_SPEED)
		};
		const float B
		{
			2.0f *
			((toTargetDiff.x * MOVE.x) + (toTargetDiff.y * MOVE.y) + (toTargetDiff.z * MOVE.z))
		};
		const float C
		{
			toTargetDiff.x * toTargetDiff.x + toTargetDiff.y * toTargetDiff.y + toTargetDiff.z * toTargetDiff.z
		};

		// 0除算防止
		if (std::fabsf(A) <= FLT_EPSILON)
		{
			if (std::fabsf(B) <= FLT_EPSILON)
			{
				return _targetPosition;
			}
			else
			{
				return _targetPosition + MOVE * (-C / B);
			}
		}

		// 当たる秒数
		float sec{};
		// 解
		float s1{}, s2{};

		// 2次方程式の解の公式 判別式D
		const float D{ B * B - 4.0f * A * C };
		if (D > FLT_EPSILON)
		{
			const float E = std::sqrtf(D);
			s1 = (-B - E) / (2.0f * A);
			s2 = (-B + E) / (2.0f * A);
			sec = PlusMin(s1, s2);
		}
		else  // 虚数解 == 当たらない
		{
			sec = 0;
		}

		return _targetPosition + MOVE * (sec);
	}
}
