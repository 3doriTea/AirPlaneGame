#pragma once
#include <functional>
#include "GameObjectLayer.h"
namespace mtgb
{
	/// <summary>
	/// UI描画時に指定する設定
	/// </summary>
	struct UIParams
	{
		int depth = 0;// 小さいほど奥、大きいほど手前
		GameObjectLayerFlag layerFlag = mtbit::operator|(GameObjectLayer::A, GameObjectLayer::B);// 描画するレイヤー
	};
	struct UIDrawCommand
	{
		UIParams params;
		std::function<void()> drawFunction; //描画関数

		bool operator<(const UIDrawCommand& other) const
		{
			return params.depth < other.params.depth;
		}
	};
}