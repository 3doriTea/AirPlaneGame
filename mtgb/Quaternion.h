#pragma once
#include <DirectXMath.h>
#include <string>


namespace mtgb
{
	struct Quaternion : DirectX::XMVECTORF32
	{
		static const size_t COUNT{ 4 };

		inline const float& X() const { return f[0]; }
		inline const float& Y() const { return f[1]; }
		inline const float& Z() const { return f[2]; }
		inline const float& W() const { return f[3]; }
		inline float& X() { return f[0]; }
		inline float& Y() { return f[1]; }
		inline float& Z() { return f[2]; }
		inline float& W() { return f[3]; }

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

		//static Quaternion Identity() { return DirectX::XMQuaternionIdentity(); }
		//static Quaternion Euler(const Vector3& _vec) { return DirectX::XMQuaternionRotationRollPitchYaw(_vec.x, _vec.y, _vec.z); }
		//Quaternion GetInverse() const { return { -X(), -Y(), -Z(), W() }; }
		//static Quaternion GetInverse(const Quaternion& _q) { return _q.GetInverse(); };
		//static float GetSize(const Quaternion& _q) { return _q.X() * _q.X() + _q.Y() * _q.Y() + _q.Z() * _q.Z() + _q.W() * _q.W(); }

		/// <summary>
		/// •¶Žš—ñ‚É•ÏŠ·‚·‚é
		/// </summary>
		/// <returns>•¶Žš—ñ</returns>
		std::string ToString() const;

		inline Quaternion& operator+=(const Quaternion& _other) { f[0] += f[0]; f[1] += f[1]; f[2] += f[2]; f[3] += f[3]; return *this; }
		inline Quaternion& operator-=(const Quaternion& _other) { f[0] -= f[0]; f[1] -= f[1]; f[2] -= f[2]; f[3] -= f[3]; return *this; }
	};
}
