#include "DrawScreenUtility.h"
#include "Game.h"
#include "Vector2.h"
#include "Screen.h"

namespace
{
	// UIê›åvíÜÇÃâëúìx
	const mtgb::Vector2 CANVAS_SIZE{ 1920.0f, 1080.0f };
}

//const mtgb::RectF mtgb::GenDrawScreenFrom(const RectF& _to)
//{
//	static const RectF RATE
//	{
//		_to.point.x / CANVAS_SIZE.x, _to.point.y / CANVAS_SIZE.y,
//		_to.size.x / CANVAS_SIZE.x, _to.size.y / CANVAS_SIZE.y,
//	};
//	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
//
//	return
//	{
//		RATE.point.x * SCREEN_SIZE.x, RATE.point.y * SCREEN_SIZE.y,
//		RATE.size.x * SCREEN_SIZE.x, RATE.size.y * SCREEN_SIZE.y,
//	};
//
//}
