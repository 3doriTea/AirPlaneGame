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

		/// <summary>
		/// �P�ʎl�������擾
		/// </summary>
		/// <returns>�P��</returns>
		static Quaternion Identity() { return DirectX::XMQuaternionIdentity(); }

		//static 

		/// <summary>
		/// �I�C���[�p���l�����ɕϊ�
		/// </summary>
		/// <param name="_vec">�I�C���[�p�̃x�N�g��</param>
		/// <returns>�l����</returns>
		static Quaternion Euler(const Vector3& _vec) { return DirectX::XMQuaternionRotationRollPitchYaw(_vec.x, _vec.y, _vec.z); }
		Quaternion GetInverse() const { return { -X(), -Y(), -Z(), W() }; }
		static Quaternion GetInverse(const Quaternion& _q) { return _q.GetInverse(); }
		float GetSize() const { return X() * X() + Y() * Y() + Z() * Z() + W() * W(); }
		static float GetSize(const Quaternion& _q) { return _q.GetSize(); }

		static Quaternion SLerp(const Vector3& _self, const Vector3& _to, float _lerp)
		{
			using DirectX::XMVECTOR;

			return DirectX::XMQuaternionSlerp(Vector3::Normalize(_self), Vector3::Normalize(_to), _lerp);
		}

		/// <summary>
		/// ������ɕϊ�����
		/// </summary>
		/// <returns>������</returns>
		std::string ToString() const;

		inline Quaternion& operator*= (const Quaternion& _other) { *this = DirectX::XMQuaternionMultiply(*this, _other); return *this; };
		inline Quaternion& operator+=(const Quaternion& _other) { f[0] += f[0]; f[1] += f[1]; f[2] += f[2]; f[3] += f[3]; return *this; }
		inline Quaternion& operator-=(const Quaternion& _other) { f[0] -= f[0]; f[1] -= f[1]; f[2] -= f[2]; f[3] -= f[3]; return *this; }
	};
}
