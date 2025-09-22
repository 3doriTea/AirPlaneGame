#pragma once
#include "ComponentPool.h"
#include "Collider.h"

#include <vector>
#include "WindowContext.h"
#include "RectContainsInfo.h"
#include "RectF.h"
#include "GameObjectTag.h"

namespace mtgb
{
	class Collider;
	class GameObject;
	class ColliderCP : public ComponentPool<Collider, false>
	{
	public:
		ColliderCP();
		~ColliderCP();

		void Start() override;
		void Update() override;

		void TestDraw() const;

		EntityId RayCastHitAll(const Vector3& _origin, const Vector3& _dir, float dist);
		bool RayCastHit(const Vector3& _origin, const Vector3& _dir, float* dist, EntityId _entityId);
		void IsHitAll(const Vector3& _center, float _radius, std::vector<EntityId>* _entityIds);

		
		/// <summary>
		/// <para> ゲームオブジェクトをウィンドウのスクリーン座標系へ変換、引数で渡された矩形に含まれているか判定 </para>
		/// <para>  含まれている場合はコレクションに格納される </para>
		/// <para>  ウィンドウの後ろに位置する場合でも格納されるので、ウィンドウに映っているか判定する場合はzが0から1かどうか確認すること </para>
		/// </summary>
		/// <param name="_rect">含まれているかの判定に使われる矩形</param>
		/// <param name="_name">判定されるゲームオブジェクトの名前</param>
		/// <param name="_info">判定されたゲームオブジェクトの情報を格納するコレクションのポインタ</param>
		/// <param name="_context">スクリーン座標系への変換に利用するウィンドウ</param>
		void RectContains(const RectF& _rect, const std::string& _name, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context);
		
		/// <summary>
		/// <para> ゲームオブジェクトをウィンドウのスクリーン座標系へ変換、引数で渡された矩形に含まれているか判定 </para>
		/// <para>  含まれている場合はコレクションに格納される </para>
		/// <para>  ウィンドウの後ろに位置する場合でも格納されるので、ウィンドウに映っているか判定する場合はzが0から1かどうか確認すること </para> 
		/// </summary>
		/// <param name="_rect">含まれているかの判定に使われる矩形</param>
		/// <param name="_tag">判定されるゲームオブジェクトのタグ</param>
		/// <param name="_info">判定されたゲームオブジェクトの情報を格納するコレクションのポインタ</param>
		/// <param name="_context">スクリーン座標系への変換に利用するウィンドウ</param>
		void RectContains(const RectF& _rect, GameObjectTag _tag, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context);

	private:
		void RectContainsImpl(const RectF& _rect,const std::vector<GameObject*>& _objs, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context);
	};
}
