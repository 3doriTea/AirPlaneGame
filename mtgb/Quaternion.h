#pragma once
#include <DirectXMath.h>
#include <string>
#include "Vector3.h"


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
			X() = _x;
			Y() = _y;
			Z() = _z;
			W() = _w;
		}
		Quaternion(const DirectX::XMVECTORF32& _v)
		{
			v = _v;
		}
		Quaternion(const DirectX::XMVECTOR& _v)
		{
			v = _v;
		}

		/// <summary>
		/// 単位四元数を取得
		/// </summary>
		/// <returns>単位</returns>
		static Quaternion Identity() { return DirectX::XMQuaternionIdentity(); }

		//static 

		/// <summary>
		/// オイラー角を四元数に変換
		/// </summary>
		/// <param name="_vec">オイラー角のベクトル</param>
		/// <returns>四元数</returns>
		static Quaternion Euler(const Vector3& _vec) { return DirectX::XMQuaternionRotationRollPitchYaw(_vec.x, _vec.y, _vec.z); }
		Quaternion GetInverse() const { return { -X(), -Y(), -Z(), W() }; }
		static Quaternion GetInverse(const Quaternion& _q) { return _q.GetInverse(); }
		float GetSize() const { return X() * X() + Y() * Y() + Z() * Z() + W() * W(); }
		static float GetSize(const Quaternion& _q) { return _q.GetSize(); }

		static Quaternion SLerp(const Quaternion& _self, const Quaternion& _to, float _lerp)
		{
			return DirectX::XMQuaternionSlerp(_self, _to, _lerp);
		}

		static Quaternion LookRotation(const Vector3& _dir, const Vector3& _upVec);

		/// <summary>
		/// 今向いている方向と、これから向きたい方向から四元数を作成
		/// </summary>
		/// <param name="_fromDir">今向いている方向ベクトル</param>
		/// <param name="_toDir">向きたい方向ベクトル</param>
		/// <returns>四元数</returns>
		static Quaternion FromToRotation(const Vector3& _fromDir, const Vector3& _toDir)
		{
			// MEMO: 1. 正規化(v1, v2)
			// MEMO: 2. 外積で回転軸の正規化(v = (x, y, z))ゲット norm(Vf x Vt)
			// MEMO: 3. 内積で回転角度(th)ゲット acos(Vf ・ Vt)
			// MEMO: 4. 四元数生成 
			// MEMO: -. q = (x sin(th / 2), y sin(th / 2), z sin(th / 2), cos(th / 2))

			Vector3 v1{ Vector3::Normalize(_fromDir) };
			Vector3 v2{ Vector3::Normalize(_toDir) };

			Vector3 axis{ DirectX::XMVector3Cross(v1, v2) };
			float th{ acosf(DirectX::XMVector3Dot(v1, v2).m128_f32[0]) };
			float s{ sinf(th * 0.5f) };
			float w{ cosf(th * 0.5f) };

			return { axis.x * s, axis.y * s, axis.z * s, w };
		}

		/// <summary>
		/// 文字列に変換する
		/// </summary>
		/// <returns>文字列</returns>
		std::string ToString() const;

		inline Quaternion& operator*= (const Quaternion& _other) { *this = DirectX::XMQuaternionMultiply(*this, _other); return *this; };
		inline Quaternion& operator+=(const Quaternion& _other) { f[0] += f[0]; f[1] += f[1]; f[2] += f[2]; f[3] += f[3]; return *this; }
		inline Quaternion& operator-=(const Quaternion& _other) { f[0] -= f[0]; f[1] -= f[1]; f[2] -= f[2]; f[3] -= f[3]; return *this; }
	};
}
