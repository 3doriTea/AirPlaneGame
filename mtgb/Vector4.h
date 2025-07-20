#pragma once
#include <DirectXMath.h>

namespace mtgb
{
	/// <summary>
	/// 4次元ベクトル
	/// </summary>
	struct Vector4 : DirectX::XMVECTORF32
	{
		/// <summary>
		/// 4次元ベクトル
		/// </summary>
		/// <param name="_x">x成分</param>
		/// <param name="_y">y成分</param>
		/// <param name="_z">z成分</param>
		/// <param name="_w">w成分</param>
		Vector4(
			const float _x,
			const float _y,
			const float _z,
			const float _w);

		Vector4() :
			Vector4{ 0.0f, 0.0f, 0.0f, 0.0f }
		{}
	};
}
