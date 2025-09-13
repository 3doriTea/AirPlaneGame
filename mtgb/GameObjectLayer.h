#pragma once
#include "MTBit/BitFlag.h"

namespace mtgb
{
	enum struct GameObjectLayer : unsigned char
	{
		A,
		B,
		SceneView, // ImGui‚ÌƒQ[ƒ€‰æ–Ê‚Ì‚İ‚É•\¦
	};

	using GameObjectLayerFlag = mtbit::BitFlag<GameObjectLayer>;
}

