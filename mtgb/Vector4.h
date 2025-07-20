#pragma once
#include <DirectXMath.h>

namespace mtgb
{
	/// <summary>
	/// 4�����x�N�g��
	/// </summary>
	struct Vector4 : DirectX::XMVECTORF32
	{
		/// <summary>
		/// 4�����x�N�g��
		/// </summary>
		/// <param name="_x">x����</param>
		/// <param name="_y">y����</param>
		/// <param name="_z">z����</param>
		/// <param name="_w">w����</param>
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
