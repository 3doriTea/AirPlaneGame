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

//			massert(false && "Transform�̃R�s�[����");
			return *this;
		}

		/// <summary>
		/// �v�Z����
		/// </summary>
		void Compute();

		void GenerateWorldMatrix(Matrix4x4* _pMatrix) const;
		void GenerateWorldRotationMatrix(Matrix4x4* _pMatrix) const;
		void GenerateParentMatrix(Matrix4x4* _pMatrix) const;
		void GenerateParentRotationMatrix(Matrix4x4* _pMatrix) const;

		/// <summary>
		/// �e��Transform���擾
		/// </summary>
		/// <returns>Transform�̃|�C���^</returns>
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
	//	Vector3 position{ Vector3::Zero() };          // ���[�J�����W
	//	Vector3 scale{ Vector3::One() };              // ���[�J���傫��
	//private:
	//	EntityId parent{ INVALD_ENTITY };  // �e�̃G���e�B�e�BId
	//public:
	//	Quaternion rotate{ Quaternion::Identity() };  // ���[�J����]�p(Degree)

	private:
		void GenerateWorldMatrixSelf(Matrix4x4* _pMatrix) const;
		void GenerateWorldRotMatrixSelf(Matrix4x4* _pMatrix) const;

	private:
		Matrix4x4 matrixTranslate_{};         // �v�Z���ꂽ�ړ��s��
		Matrix4x4 matrixRotate_{};            // �v�Z���ꂽ��]�s��
		Matrix4x4 matrixScale_{};             // �v�Z���ꂽ�g�k�s��
		Matrix4x4 matrixWorld_{};             // �v�Z���ꂽ���[���h�s��
		Matrix4x4 matrixWorldRot_{};          // �v�Z���ꂽ���[���h��]�s��
	};
}
