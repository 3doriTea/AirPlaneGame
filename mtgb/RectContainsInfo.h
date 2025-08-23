#pragma once
#include "Vector3.h"
#include "Vector2F.h"
namespace mtgb
{
	/// <summary>
	/// 矩形に含まれている対象の情報
	/// </summary>
	struct RectContainsInfo
	{
		Vector3 worldPos{};//ワールド座標での座標
		Vector2F screenPos{};//スクリーン座標系での座標
		RectContainsInfo() = default;
		RectContainsInfo(Vector3 _worldPos, Vector2F _screenPos)
			:worldPos{ _worldPos }
			,screenPos{ _screenPos}
		{ }
	};
}