#pragma once
#include <cmtgb.h>
#include "Vector3.h"
#include "Quaternion.h"

namespace mtgb
{
	using EntityId = int64_t;

	struct TransformCore
	{
		inline TransformCore():
			parent{ INVALD_ENTITY },
			position{},
			scale{},
			rotate{}
		{}

		EntityId parent;
		Vector3 position;
		Vector3 scale;
		Quaternion rotate;
	};
}
