#pragma once
//#include "World.h"
#include <list>
#include "Handlers.h"
#include <string>

namespace mtgb
{
	class GameObject;
	class Transform;

	//using GameScene = World;
	class GameScene
	{
		friend class SceneSystem;
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
}
