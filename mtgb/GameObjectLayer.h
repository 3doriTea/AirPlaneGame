#pragma once
#include "MTBit/BitFlag.h"

namespace mtgb
{
	enum struct GameObjectLayer : unsigned char
	{
		A,
		B,
	};

	using GameObjectLayerFlag = mtbit::BitFlag<GameObjectLayer>;
}

