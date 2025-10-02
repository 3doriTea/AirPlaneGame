#pragma once
//#include "World.h"
#include <cmtgb.h>
#include <list>
#include "Handlers.h"
#include <string>
#include <vector>
#include <type_traits>
#include "GameObjectTag.h"
namespace mtgb
{
	using EntityId = int64_t;

	class GameObject;
	class Transform;

	//using GameScene = World;
	class GameScene
	{
		friend class SceneSystem;
		friend class RenderSystem;
		friend class Game;
	private:
		static GameScene* pInstance_;

	public:
		GameScene();
		virtual ~GameScene();

		template<class GameObjectT, typename ...Args>
		GameObjectT* Instantiate(Args... _args);

		/// <summary>
		/// カメラオブジェクトとして登録する
		/// </summary>
		/// <param name="_pGameObject">登録するゲームオブジェクトのポインタ</param>
		/// <returns>登録されたカメラハンドル</returns>
		CameraHandleInScene RegisterCameraGameObject(GameObject* _pGameObject) const;
		
		GameObject* GetGameObject(std::string _name) const;
		void GetGameObjects(const std::string& _name, std::vector<GameObject*>* _pFoundGameObjects) const;
		void GetGameObjects(GameObjectTag _tag, std::vector<GameObject*>* _pFoundGameObjects) const;

		/// <summary>
		/// テンプレートパラメータでオブジェクトを取得
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns>存在していない、GameObjectを継承していないならnullptr</returns>
		template<typename T>
		T* GetGameObject() const;

		template<typename T>
		void GetGameObjects(std::vector<T*>* _pFoundGameObjects) const;


		/// <summary>
		/// EntityIdでオブジェクトを取得
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns>存在しないならnullptr</returns>
		GameObject* GetGameObject(const EntityId _entityId) const;


	protected:
		virtual void Initialize();
		virtual void Update();
		virtual void Draw() const;
		virtual void End();

	private:
		std::list<GameObject*> pGameObjects_;  // シーンに登場するゲームオブジェクト
	};

	template<class GameObjectT, typename ...Args>
	inline GameObjectT* GameScene::Instantiate(Args... _args)
	{
		// 基底クラスがGameObjectであるか
		static_assert(std::is_base_of<GameObject, GameObjectT>().value
			&& "GameObjectクラスを継承していないクラスはインスタンスできません。");

		GameObjectT* pInstance{ new GameObjectT{ _args... } };

		// TODO: ここも連続した配列にする
		pGameObjects_.push_back(pInstance);

		return pInstance;
	}
	template<typename T>
	inline T* GameScene::GetGameObject() const
	{
		// 基底クラスがGameObjectであるか
		if (std::is_base_of<GameObject, std::remove_cvref_t<T>>().value == false)
		{
			return nullptr;
		}
	
		for (GameObject* obj : pGameObjects_)
		{
			T* instance = dynamic_cast<T*>(obj);

			if (instance)
			{
				return instance;
			}
		}
		return nullptr;
	}
	template<typename T>
	inline void GameScene::GetGameObjects(std::vector<T*>* _pFoundGameObjects) const
	{
		_pFoundGameObjects->clear();

		// 基底クラスがGameObjectであるか
		if (std::is_base_of<GameObject, std::remove_cvref_t<T>>().value == false)
		{
			return;
		}

		for (GameObject* obj : pGameObjects_)
		{
			T* instance = dynamic_cast<T*>(obj);

			if (instance)
			{
				_pFoundGameObjects->push_back(instance);
			}
		}
	}
}
