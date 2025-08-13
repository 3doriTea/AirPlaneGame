#pragma once
#include "IComponent.h"
#include "Vector3.h"
#include "Vector3Ref.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "cmtgb.h"
#include "TransformCP.h"
#include "TransformCore.h"

namespace mtgb
{
	using EntityId = int64_t;

	class TransformCP;

	class Transform : public IComponent<mtgb::TransformCP, Transform>, public TransformCore
	{
		friend TransformCP;

	public:
		using IComponent<mtgb::TransformCP, Transform>::IComponent;
		//Transform();
		~Transform();
		inline Transform& operator=(const Transform& _other)
		{
			if (&_other == this)
			{
				return *this;
			}

			this->parent = _other.parent;
			this->position = _other.position;
			this->scale = _other.scale;
			this->rotate = _other.rotate;

			this->matrixTranslate_ = _other.matrixTranslate_;
			this->matrixRotate_ = _other.matrixRotate_;
			this->matrixScale_ = _other.matrixScale_;
			this->matrixWorld_ = _other.matrixWorld_;

//			massert(false && "Transformのコピー発生");
			return *this;
		}

		/// <summary>
		/// 計算する
		/// </summary>
		void Compute();

		void GenerateWorldMatrix(Matrix4x4* _pMatrix) const;
		void GenerateWorldRotationMatrix(Matrix4x4* _pMatrix) const;
		void GenerateParentMatrix(Matrix4x4* _pMatrix) const;
		void GenerateParentRotationMatrix(Matrix4x4* _pMatrix) const;

		/// <summary>
		/// 親のTransformを取得
		/// </summary>
		/// <returns>Transformのポインタ</returns>
		Transform* GetParent() const;
		void SetParent(const EntityId _entityId) { parent = _entityId; }

		void Rotation(const Vector3& _rotate);

		Vector3 Up() const { return Vector3::Up() * matrixWorldRot_; }
		Vector3 Down() const { return Vector3::Down() * matrixWorldRot_; }
		Vector3 Left() const { return Vector3::Left() * matrixWorldRot_; }
		Vector3 Right() const { return Vector3::Right() * matrixWorldRot_; }
		Vector3 Back() const { return Vector3::Back() * matrixWorldRot_; }
		Vector3 Forward() const;

	//public:
	//	Vector3 position{ Vector3::Zero() };          // ローカル座標
	//	Vector3 scale{ Vector3::One() };              // ローカル大きさ
	//private:
	//	EntityId parent{ INVALD_ENTITY };  // 親のエンティティId
	//public:
	//	Quaternion rotate{ Quaternion::Identity() };  // ローカル回転角(Degree)

	private:
		void GenerateWorldMatrixSelf(Matrix4x4* _pMatrix) const;
		void GenerateWorldRotMatrixSelf(Matrix4x4* _pMatrix) const;

	private:
		Matrix4x4 matrixTranslate_{};         // 計算された移動行列
		Matrix4x4 matrixRotate_{};            // 計算された回転行列
		Matrix4x4 matrixScale_{};             // 計算された拡縮行列
		Matrix4x4 matrixWorld_{};             // 計算されたワールド行列
		Matrix4x4 matrixWorldRot_{};          // 計算されたワールド回転行列
	};
}
