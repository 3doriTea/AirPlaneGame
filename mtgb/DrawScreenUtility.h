#pragma once
//#include "RectF.h"
#include "RectT.h"
#include "Game.h"
#include "Vector2.h"
#include "Screen.h"

namespace
{
	// UI設計中の解像度
	const mtgb::Vector2 CANVAS_SIZE{ 1920.0f, 1080.0f };
}

namespace mtgb
{
	/// <summary>
	/// 設計上のスクリーン矩形から描画時のスクリーン矩形を生成する
	/// </summary>
	/// <param name="_rect">設計時のスクリーン矩形</param>
	/// <returns>描画時のスクリーン矩形</returns>
	template<typename T>
	inline const mtgb::RectT<T> GenDrawScreenFrom(const mtgb::RectT<T>& _to)
	{
		static const RectF RATE
		{
			_to.point.x / CANVAS_SIZE.x, _to.point.y / CANVAS_SIZE.y,
			_to.size.x / CANVAS_SIZE.x, _to.size.y / CANVAS_SIZE.y,
		};
		const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

		return
		{
			RATE.point.x * SCREEN_SIZE.x, RATE.point.y * SCREEN_SIZE.y,
			RATE.size.x * SCREEN_SIZE.x, RATE.size.y * SCREEN_SIZE.y,
		};

	}

	template<typename T>
	inline const mtgb::Vector2T<T> GenDrawScreenFrom(const mtgb::Vector2T<T>& _to)
	{
		static const Vector2 RATE{ _to.x / CANVAS_SIZE.x, _to.y / CANVAS_SIZE.y };
		const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

		return { SCREEN_SIZE.x * RATE.x, SCREEN_SIZE.y * RATE.y };
	}
}
