#pragma once
#include <functional>
#include "GameObjectLayer.h"
namespace mtgb
{
	struct UIParams
	{
		int depth = 0;
		GameObjectLayerFlag layerFlag = GameObjectLayer::All;
	};
	struct UIDrawCommand
	{
		UIParams params;
		std::function<void()> drawFunction; //ï`âÊä÷êî

		bool operator<(const UIDrawCommand& other) const
		{
			return params.depth < other.params.depth;
		}
	};
}