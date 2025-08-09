#pragma once
#include "Vector3.h"
#include "Quaternion.h"

namespace mtgb
{
	using EntityId = int64_t;

	struct TransformCore
	{
		EntityId parent;
		Vector3 position;
		Vector3 scale;
		Quaternion rotate;
	};
}
