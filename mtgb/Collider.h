#pragma once
#include "IComponent.h"
#include "ColliderCP.h"
#include <set>
#include <DirectXCollision.h>
#include "Vector3.h"


namespace mtgb
{
	class ColliderCP;
	class Transform;

	class Collider : public IComponent<ColliderCP, Collider>
	{
		friend ColliderCP;

	public:

		enum ColliderType : uint8_t
		{
			TYPE_SPHERE,  // 球(中心からの一定距離)
			TYPE_CAPSULE,  // カプセル(線分からの一定距離)
			TYPE_AABB,	  // 軸並行境界ボックス(各軸に平行な辺)
		};

		// 衝突判定をするか否かのタグ
		enum class ColliderTag
		{
			GAME_OBJECT, 
			STAGE,
			STAGE_BOUNDARY, // ステージの境界、範囲外のコライダーを意味するタグ
		};

	public:
		using IComponent<ColliderCP, Collider>::IComponent;
		Collider(EntityId _entityId);
		Collider(EntityId _entityId, ColliderTag _colliderTag);
		~Collider();
		inline Collider& operator= (const Collider& _other)
		{
			if (&_other == this)
			{
				return *this;
			}



			return *this;
		}

		bool IsHit(const Collider& _other) const;
		/// <summary>
		/// レイとの交差判定
		/// </summary>
		/// <param name="_origin">原点</param>
		/// <param name="_dir">方向(内部で正規化される)</param>
		/// <param name="dist">
		/// <para> レイの原点からコライダーとの最初の交点までの距離を格納。</para>
		/// <para> レイの原点が球の内側の場合は、球を出る点までの距離</para>
		/// </param>
		/// <returns></returns>
		bool IsHit(const Vector3& _origin, const Vector3& _dir, float* dist);
		bool IsHit(const Vector3& _center, float _radius) const;
		void Draw() const;

		// BoundingSphereを初期化
		void UpdateBoundingData();
		
		void SetCenter(const Vector3& _center);
		void SetExtents(const Vector3& _extents);
		void SetRadius(float _radius);
		ColliderTag GetColliderTag() const { return colliderTag_; }
	public:
		ColliderType type_;  // 当たり判定の形

		// 静的な、Transform不要なコライダー用のフラグ
		bool isStatic_;

		//union
		//{
		//	struct
		//	{
		//		Vector3 offset_;  // オフセット
		//		float radius_ = 1.0f;  // 半径
		//	} sphere_;
		//	struct
		//	{
		//		Vector3 center_;  // AABBの中心
		//		Vector3 extents_; // AABBのサイズの半分
		//	} aabb_;
		//	struct
		//	{
		//		// カプセルのデータ
		//	} capsule_;
		//};

		std::set<Collider*> onColliders_;

		
	private:
		void UpdateBoundingSphere();
		void UpdateBoundingBox();
		union
		{
			DirectX::BoundingSphere computeSphere_;
			DirectX::BoundingBox computeBox_;
		};

		Transform* pTransform_;  // TODO: 危ないTransform
		ColliderTag colliderTag_;
		static FBXModelHandle hSphereModel_;
		static FBXModelHandle hBoxModel_;
	};
}
