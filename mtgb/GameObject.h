#pragma once
#include "Entity.h"
#include <string>
#include "GameObjectBuilder.h"
#include <bitset>
#include <vector>
#include "Transform.h"

constexpr size_t COMPONENT_CAPACITY{ (8 * 8) - 4 };

namespace mtgb
{
	class SceneSystem;

	class GameObject : public Entity
	{
	public:
		GameObject(const GAME_OBJECT_DESC& _desc);
		GameObject(const GameObject& _other);
		virtual ~GameObject();

		virtual inline void Initialize() {};
		virtual inline void Update() {};
		virtual inline void Draw() const {};
		virtual inline void End() {};

		/// <summary>
		/// 名前を設定する
		/// </summary>
		/// <param name="_name"></param>
		void SetName(const std::string _name) { name_ = _name; }

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

		template<typename T>
		T* FindGameObject() const;

		template<typename T>
		void FindGameObjects(std::vector<T*>* _pFoundGameObjects);

		GameObject* FindGameObject(const std::string& _name);
		void FindGameObjects(const std::string& _name, std::vector<GameObject*>* _pFoundGameObjects);
		GameObject* FindGameObject(const EntityId _entityId);

		/// <summary>
		/// このオブジェクトを削除する
		/// </summary>
		inline void DestroyMe() { status_.toDestroy_ = TRUE; }
		/// <summary>
		/// このオブジェクトは削除予定か
		/// </summary>
		/// <returns>削除予定 true / false</returns>
		inline const bool IsToDestroy() const { return status_.toDestroy_; }
		std::string GetName() { return name_; }
		/// <summary>
		/// レイヤーフラグを取得
		/// </summary>
		/// <returns>レイヤーフラグを取得する</returns>
		GameObjectLayerFlag GetLayerFlag() const { return layerFlag_; }

	protected:

		std::string name_;

		struct Status
		{
			uint8_t isActive_ : 1;
			uint8_t callUpdate_ : 1;
			uint8_t callDraw_ : 1;
			uint8_t toDestroy_ : 1;  // 削除予定か
		} status_;

		GameObjectLayerFlag layerFlag_;  // レイヤーのフラグ


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
	template<typename GameSceneT>
	inline GameSceneT& GameObject::GetScene()
	{
		GameSceneT* pActiveScene{ Game::System<SceneSystem>().GetActiveScene() };
		massert(pActiveScene != nullptr
			&& "アクティブシーンの取得に失敗 @GameObject::GetScene");

		return *pActiveScene;
	}
	template<typename T>
	inline T* GameObject::FindGameObject() const
	{
		// templateキーワードを付けないと<T>の<が小なりと区別がつかない?
		return mtgb::Game::System<SceneSystem>().GetActiveScene()->template GetGameObject<T>();
	}
	template<typename T>
	inline void GameObject::FindGameObjects(std::vector<T*>* _pFoundGameObjects)
	{
		// templateキーワードを付けないと<T>の<が小なりと区別がつかない?
		mtgb::Game::System<SceneSystem>().GetActiveScene()->template GetGameObjects<T>(_pFoundGameObjects);
	}
}
