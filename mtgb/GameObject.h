#pragma once
#include "Entity.h"
#include <string>
#include "GameObjectBuilder.h"
#include <bitset>


constexpr size_t COMPONENT_CAPACITY{ (8 * 8) - 4 };

namespace mtgb
{
	class SceneSystem;

	class GameObject : public Entity
	{
	public:
		GameObject(const GAME_OBJECT_DESC& _desc);
		virtual ~GameObject();

		virtual inline void Initialize() {};
		virtual inline void Update() {};
		virtual inline void Draw() const {};
		virtual inline void End() {};

		/// <summary>
		/// コンポーネントを取得/追加
		/// </summary>
		/// <typeparam name="ComponentT">コンポーネントの型</typeparam>
		/// <returns>コンポーネントのポインタ</returns>
		template<class ComponentT>
		ComponentT* Component();

		/// <summary>
		/// ゲームオブジェクトをインスタンスする
		/// </summary>
		/// <typeparam name="GameObjectT">ゲームオブジェクトの型</typeparam>
		/// <typeparam name="...Args">コンストラクタ可変長引数型</typeparam>
		/// <param name="..._args">コンストラクタ可変長引数</param>
		/// <returns>インスタンスされたゲームオブジェクトのポインタ</returns>
		template<class GameObjectT, typename ...Args>
		GameObjectT* Instantiate(Args... _args);

		template<typename GameSceneT>
		GameSceneT& GetScene();

		GameObject* FindGameObject(const std::string& _name);

		/// <summary>
		/// このオブジェクトを削除する
		/// </summary>
		inline void DestoryMe() { status_.toDestory_ = TRUE; }
		/// <summary>
		/// このオブジェクトは削除予定か
		/// </summary>
		/// <returns>削除予定 true / false</returns>
		inline const bool IsToDestory() const { return status_.toDestory_; }

	private:
		struct Status
		{
			uint8_t isActive : 1;
			uint8_t callUpdate_ : 1;
			uint8_t callDraw_ : 1;
			uint8_t toDestory_ : 1;  // 削除予定か
		} status_;


		std::bitset<COMPONENT_CAPACITY> componentsFlag_;  // コンポーネントのフラグ
	};

	template<class ComponentT>
	inline ComponentT* GameObject::Component()
	{
		return &ComponentT::Get(entityId_);
	}

	template<class GameObjectT, typename ...Args>
	inline GameObjectT* GameObject::Instantiate(Args ..._args)
	{
		return Game::System<SceneSystem>().GetActiveScene()
			->Instantiate<GameObjectT>(_args...);
	}
}
