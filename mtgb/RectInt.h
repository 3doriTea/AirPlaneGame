#pragma once
#include "cmtgb.h"
#include "Vector2Int.h"

namespace mtgb
{
	/// <summary>
	/// <para>矩形構造体</para>
	/// <para>始点とサイズを持っている</para>
	/// <para>終点は関数を介して取得可能</para>
	/// </summary>
	struct RectInt
	{
		RectInt() :
			x{ 0 },
			y{ 0 },
			width{ 0 },
			height{ 0 }
		{}
		RectInt(
			const int32_t _x,
			const int32_t _y,
			const int32_t _width,
			const int32_t _height) :
			x{ _x },
			y{ _y },
			width{ _width },
			height{ _height }
		{}
		RectInt(const Vector2Int& _point, const Vector2Int& _size) :
			point{ _point },
			size{ _size }
		{}

		union
		{
			struct
			{
				int32_t x;  // 点 x
				int32_t y;  // 点 y
			};
			Vector2Int point;  // 軸点
		};
		union
		{
			struct
			{
				int32_t width;   // ヨコ幅
				int32_t height;  // タテ幅
			};
			Vector2Int size;  // サイズ
		};

		/// <summary>
		/// 始点を取得 (最小点)
		/// </summary>
		/// <returns>始点の座標</returns>
		inline Vector2Int GetBegin() const
		{
			return
			{
				(size.x < 0) ? (point.x + size.x) : (point.x),
				(size.y < 0) ? (point.y + size.y) : (point.y)
			};
		}
		/// <summary>
		/// 終点を取得 (最大点)
		/// </summary>
		/// <returns>終点の座標</returns>
		inline Vector2Int GetEnd() const
		{
			return
			{
				(size.x > 0) ? (point.x + size.x) : (point.x),
				(size.y > 0) ? (point.y + size.y) : (point.y)
			};
		}

		/// <summary>
		/// 矩形を反転させる
		/// </summary>
		/// <param name="_rect">反転させたい矩形</param>
		/// <returns>反転された矩形</returns>
		static inline RectInt Reverse(const RectInt& _rect)
		{
			Vector2Int reversePoint{ _rect.point + _rect.size };
			Vector2Int reverseSize{ _rect.size * -1 };

			return { reversePoint, reverseSize };
		}
		/// <summary>
		/// 矩形をx軸で反転させる
		/// </summary>
		/// <param name="_rect">x軸で反転させたい矩形</param>
		/// <returns>反転された矩形</returns>
		static inline RectInt ReverseAxisX(const RectInt& _rect)
		{
			Vector2Int reversePoint
			{
				_rect.point.x,
				_rect.point.y + _rect.size.y
			};
			Vector2Int reverseSize
			{
				_rect.size.x,
				_rect.size.y * -1,
			};

			return { reversePoint, reverseSize };
		}

		/// <summary>
		/// 線分から矩形をインスタンス
		/// </summary>
		/// <param name="_begin">始点座標</param>
		/// <param name="_end">終点座標</param>
		/// <returns>矩形構造体</returns>
		static inline RectInt FromLine(
			const Vector2Int& _begin,
			const Vector2Int& _end)
		{
			return
			{
				_begin,
				_end - _begin
			};
		}
	};
}
