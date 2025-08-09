#pragma once
#include "IComponent.h"
#include "Vector3.h"
#include "Vector3Ref.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "cmtgb.h"
#include "TransformCP.h"

namespace mtgb
{
	using EntityId = int64_t;

	class TransformCP;

	class Transform : public IComponent<mtgb::TransformCP, Transform>
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

			this->position_ = _other.position_;
			this->rotate_ = _other.rotate_;
			this->scale_ = _other.scale_;

			this->parentId_ = _other.parentId_;
			this->matrixTranslate_ = _other.matrixTranslate_;
			this->matrixRotate_ = _other.matrixRotate_;
			this->matrixScale_ = _other.matrixScale_;
			this->parentId_ = _other.parentId_;

//			massert(false && "Transformのコピー発生");
			return *this;
		}

		void GenerateWorldMatrix(Matrix4x4* _pMatrix) const;
		void GenerateWorldRotationMatrix(Matrix4x4* _pMatrix) const;
		void GenerateParentMatrix(Matrix4x4* _pMatrix) const;
		void GenerateParentRotationMatrix(Matrix4x4* _pMatrix) const;

		/// <summary>
		/// 親のTransformを取得
		/// </summary>
		/// <returns>Transformのポインタ</returns>
		Transform* GetParent() const;
		void SetParent(const EntityId _entityId) { parentId_ = _entityId; }

		void Rotation(const Vector3& _rotate);

		Vector3 Up() const { return Vector3::Up() * matrixRotate_; };
		Vector3 Down() const { return Vector3::Down() * matrixRotate_; };
		Vector3 Left() const { return Vector3::Left() * matrixRotate_; };
		Vector3 Right() const { return Vector3::Right() * matrixRotate_; };
		Vector3 Back() const { return Vector3::Back() * matrixRotate_; };
		Vector3 Forward() const { return Vector3::Forward() * matrixRotate_; };

	public:
		Vector3 position_{ Vector3::Zero() };          // ローカル座標
		Vector3 scale_{ Vector3::One() };              // ローカル大きさ
	private:
		EntityId parentId_{ INVALD_ENTITY };  // 親のエンティティId
	public:
		Quaternion rotate_{ Quaternion::Identity() };  // ローカル回転角(Degree)

	private:
		Matrix4x4 matrixTranslate_{};         // 計算された移動行列
		Matrix4x4 matrixRotate_{};            // 計算された回転行列
		Matrix4x4 matrixScale_{};             // 計算された拡縮行列
	};
}
