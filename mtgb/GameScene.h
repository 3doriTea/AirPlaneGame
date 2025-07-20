#pragma once
//#include "World.h"
#include <list>

namespace mtgb
{
	class SceneSystem;
	class GameObject;
	class Transform;

	//using GameScene = World;
	class GameScene
	{
		friend SceneSystem;
	private:
		static GameScene* pInstance_;

	public:
		GameScene();
		virtual ~GameScene();

		template<class GameObjectT, typename ...Args>
		GameObjectT* Instantiate(Args... _args);

		void SetCameraGameObject(GameObject* _pGameObject);
		Transform* GetCameraTransform() { return pCameraTransform_; }

	protected:
		virtual void Initialize();
		virtual void Update();
		virtual void Draw() const;
		virtual void End();

	private:
		GameObject* pCameraGameObject_;  // シーンにただ一つ存在するカメラ
		Transform* pCameraTransform_;    // シーンにただ一つ存在するカメラの座標系
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
