#pragma once
#include "MTBit/BitFlag.h"

namespace mtgb
{
	enum struct GameObjectLayer : unsigned char
	{
		A,
		B,
		All,
	};

	using GameObjectLayerFlag = mtbit::BitFlag<GameObjectLayer>;
}

