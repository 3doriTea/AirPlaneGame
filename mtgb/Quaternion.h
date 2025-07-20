#pragma once
#include <DirectXMath.h>
#include <string>


namespace mtgb
{
	struct Quaternion : DirectX::XMVECTORF32
	{
		static const size_t COUNT{ 4 };

		Quaternion() :
			Quaternion{ 0, 0, 0, 0 }
		{}
		Quaternion(float _x, float _y, float _z, float _w)
		{
			f[0] = _x;
			f[1] = _y;
			f[2] = _z;
			f[3] = _w;
		}
		Quaternion(const DirectX::XMVECTORF32& _v)
		{
			v = _v;
		}
		Quaternion(const DirectX::XMVECTOR& _v)
		{
			v = _v;
		}

		static Quaternion Identity() { return DirectX::XMQuaternionIdentity(); }

		/// <summary>
		/// •¶Žš—ñ‚É•ÏŠ·‚·‚é
		/// </summary>
		/// <returns>•¶Žš—ñ</returns>
		std::string ToString() const;
	};
}
