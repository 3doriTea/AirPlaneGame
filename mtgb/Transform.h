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

		/// <summary>
		/// ワールド行列を生成する
		/// </summary>
		/// <param name="_pMatrix">行列のポインタ渡し</param>
		void GenerateWorldMatrix(Matrix4x4* _pMatrix) const;
		/// <summary>
		/// ワールド回転行列を生成する
		/// </summary>
		/// <param name="_pMatrix">行列のポインタ渡し</param>
		void GenerateWorldRotationMatrix(Matrix4x4* _pMatrix) const;
		/// <summary>
		/// 親の行列を生成する
		/// </summary>
		/// <param name="_pMatrix">行列のポインタ渡し</param>
		void GenerateParentMatrix(Matrix4x4* _pMatrix) const;
		/// <summary>
		/// 親の回転行列を生成する
		/// </summary>
		/// <param name="_pMatrix">行列のポインタ渡し</param>
		void GenerateParentRotationMatrix(Matrix4x4* _pMatrix) const;

		/// <summary>
		/// 親のTransformを取得
		/// </summary>
		/// <returns>Transformのポインタ</returns>
		Transform* GetParent() const;
		/// <summary>
		/// 親のTransformを設定
		/// </summary>
		/// <param name="_entityId">親にするエンティティ識別子</param>
		void SetParent(const EntityId _entityId) { parent = _entityId; }

		/// <summary>
		/// 回転する
		/// </summary>
		/// <param name="_rotate">オイラー角(ラジアン)</param>
		void Rotation(const Vector3& _rotate);
		/// <summary>
		/// 軸ベクトルで回転する
		/// </summary>
		/// <param name="_axis">軸のベクトル</param>
		/// <param name="_angle">回転角度(ラジアン)</param>
		void Rotation(const Vector3& _axis, const float _angle);

		/// <summary>
		/// 上方向のワールドベクトル
		/// </summary>
		/// <returns>ワールド回転行列がかけられたベクトル</returns>
		Vector3 Up() const { return Vector3::Up() * matrixWorldRot_; }
		/// <summary>
		/// 下方向のワールドベクトル
		/// </summary>
		/// <returns>ワールド回転行列がかけられたベクトル</returns>
		Vector3 Down() const { return Vector3::Down() * matrixWorldRot_; }
		/// <summary>
		/// 左方向のワールドベクトル
		/// </summary>
		/// <returns>ワールド回転行列がかけられたベクトル</returns>
		Vector3 Left() const { return Vector3::Left() * matrixWorldRot_; }
		/// <summary>
		/// 右方向のワールドベクトル
		/// </summary>
		/// <returns>ワールド回転行列がかけられたベクトル</returns>
		Vector3 Right() const { return Vector3::Right() * matrixWorldRot_; }
		/// <summary>
		/// 後方向のワールドベクトル
		/// </summary>
		/// <returns>ワールド回転行列がかけられたベクトル</returns>
		Vector3 Back() const { return Vector3::Back() * matrixWorldRot_; }
		/// <summary>
		/// 前方向のワールドベクトル
		/// </summary>
		/// <returns>ワールド回転行列がかけられたベクトル</returns>
		Vector3 Forward() const;

		/// <summary>
		/// ワールド座標を取得
		/// </summary>
		/// <returns>ワールド座標のベクトル</returns>
		Vector3 GetWorldPosition() const { return Vector3::Zero() * matrixWorld_; }
		/// <summary>
		/// ワールド回転を取得
		/// </summary>
		/// <returns>ワールド回転の四元数</returns>
		Quaternion GetWorldRotate() const;

	//public:
	//	Vector3 position{ Vector3::Zero() };          // ローカル座標
	//	Vector3 scale{ Vector3::One() };              // ローカル大きさ
	//private:
	//	EntityId parent{ INVALD_ENTITY };  // 親のエンティティId
	//public:
	//	Quaternion rotate{ Quaternion::Identity() };  // ローカル回転角(Degree)

	private:
		/// <summary>
		/// 計算用自分自身のワールド行列を生成する
		/// </summary>
		/// <param name="_pMatrix">行列のポインタ渡し</param>
		void GenerateWorldMatrixSelf(Matrix4x4* _pMatrix) const;
		/// <summary>
		/// 計算用自分自身のワールド回転行列を生成する
		/// </summary>
		/// <param name="_pMatrix">行列のポインタ渡し</param>
		void GenerateWorldRotMatrixSelf(Matrix4x4* _pMatrix) const;

	public://private:
		Matrix4x4 matrixTranslate_{};         // 計算された移動行列
		Matrix4x4 matrixRotate_{};            // 計算された回転行列
		Matrix4x4 matrixScale_{};             // 計算された拡縮行列
		Matrix4x4 matrixWorld_{};             // 計算されたワールド行列
		Matrix4x4 matrixWorldRot_{};          // 計算されたワールド回転行列
	};
}
