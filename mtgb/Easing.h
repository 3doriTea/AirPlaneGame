#pragma once
#include <cmath>

namespace mtgb
{
	/// <summary>
	/// <para>イージング関数</para>
	/// <para>https://easings.net/ja</para>
	/// </summary>
	struct Ease
	{
		static inline float OutExpo(const float x)
		{
			// https://easings.net/ja#easeOutExpo
			return x == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * x);
		}
	};
}
