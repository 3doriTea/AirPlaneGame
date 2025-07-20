#pragma once

namespace mtgb
{
	/// <summary>
	/// Vector3‚ÌQÆ
	/// </summary>
	struct Vector3Ref
	{
		Vector3Ref(
			float& _x,
			float& _y,
			float& _z
		) :
			x{ _x },
			y{ _y },
			z{ _z }
		{}

		float& x;
		float& y;
		float& z;
	};
}
