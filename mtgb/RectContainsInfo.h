#pragma once
#include "Vector3.h"
#include "Vector2F.h"
#include "Entity.h"
namespace mtgb
{
	/// <summary>
	/// 矩形に含まれている対象の情報
	/// </summary>
	struct RectContainsInfo
	{
		Vector3 worldPos{};//ワールド座標での座標
		Vector3 screenPos{};//スクリーン座標系での座標
		EntityId entityId;
		RectContainsInfo() = default;
		RectContainsInfo(Vector3 _worldPos, Vector3 _screenPos,EntityId _entityId)
			:worldPos{ _worldPos }
			,screenPos{ _screenPos}
			,entityId{_entityId}
		{ }
	};
}