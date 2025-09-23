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

		static inline float OutQuart(const float x)
		{
			// https://easings.net/ja#easeOutQuart
			return 1.0f - std::powf(1.0f - x, 4.0f);
		}

		static inline float OutBounce(float x)
		{
			// https://easings.net/ja#easeOutBounce
			static const float n1 = 7.5625f;
			static const float d1 = 2.75f;
			
			if (x < 1.0f / d1)
			{
				return n1 * x * x;
			}
			else if (x < 2.0f / d1)
			{
				return n1 * (x -= 1.5f / d1) * x + 0.75f;
			}
			else if (x < 2.5f / d1)
			{
				return n1 * (x -= 2.25f / d1) * x + 0.9375f;
			}
			else
			{
				return n1 * (x -= 2.625f / d1) * x + 0.984375f;
			}
		}
	};
}
