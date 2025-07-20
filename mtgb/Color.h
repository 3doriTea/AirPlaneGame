#pragma once
#include <cstdint>
#include "Vector3.h"
#include "Vector4.h"

namespace mtgb
{
	/// <summary>
	/// 色を0~255の範囲で表現する構造体
	/// </summary>
	struct Color
	{
		/// <summary>
		/// 代表的な色コード
		/// </summary>
		enum : uint32_t
		{
			RED   = 0xff0000,   // 赤色
			GREEN = 0x00ff00,   // 緑色
			BLUE  = 0x0000ff,   // 青色
			WHITE  = 0xffffff,  // 白色
		};

		/// <summary>
		/// 色の各成分にアクセス用
		/// </summary>
		enum struct Component : int32_t
		{
			Blue,   // 青色成分
			Green,  // 緑色成分
			Red,    // 赤色成分
			Alpha,  // 透過度成分
			Max,    // 色成分の種類数
		};

		/// <summary>
		/// 色の各成分から色を作る
		/// </summary>
		/// <param name="_r">赤成分(0~255)</param>
		/// <param name="_g">緑成分(0~255)</param>
		/// <param name="_b">青成分(0~255)</param>
		/// <param name="_alpha">透明度成分(0~255)</param>
		Color(
			const uint8_t _r,
			const uint8_t _g,
			const uint8_t _b,
			const uint8_t _alpha = UINT8_MAX);

		/// <summary>
		/// カラーコードから色を作る
		/// </summary>
		/// <param name="_code">16進数カラーコード</param>
		Color(uint32_t _code);

		/// <summary>
		/// カラーコードをunsigned int にキャスト
		/// </summary>
		inline operator uint32_t() const noexcept { return code; }

		/// <summary>
		/// 透明度成分の正規化した値を取得
		/// </summary>
		/// <returns>透明度成分の正規化された値</returns>
		inline float GetAlphaNorm() const
		{
			return static_cast<float>(component[static_cast<int32_t>(Component::Alpha)]) / UINT8_MAX;
		}
		/// <summary>
		/// 各色成分を正規化した3次元ベクトルを取得
		/// </summary>
		/// <returns>RGBを正規化した3次元ベクトル</returns>
		inline Vector3 ToVector3Norm() const
		{
			return
			{
				static_cast<float>(component[static_cast<int32_t>(Component::Red)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Green)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Blue)]) / UINT8_MAX,
			};
		}
		/// <summary>
		/// 各色成分+透明度成分を正規化した4次元ベクトルを取得
		/// </summary>
		/// <returns>RGBAを正規化した3次元ベクトル</returns>
		inline Vector4 ToVector4Norm() const
		{
			return
			{
				static_cast<float>(component[static_cast<int32_t>(Component::Red)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Green)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Blue)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Alpha)]) / UINT8_MAX,
			};
		}

		union
		{
			uint32_t code;  // 16進数カラーコード用
			uint8_t component[sizeof(uint32_t) / sizeof(uint8_t)];  // 1バイトずつのカラー要素用
		};
	};
}
