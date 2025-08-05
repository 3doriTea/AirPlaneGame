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
		/// �J�����I�u�W�F�N�g�Ƃ��ēo�^����
		/// </summary>
		/// <param name="_pGameObject">�o�^����Q�[���I�u�W�F�N�g�̃|�C���^</param>
		/// <returns>�o�^���ꂽ�J�����n���h��</returns>
		CameraHandleInScene RegisterCameraGameObject(GameObject* _pGameObject) const;
		
		GameObject* GetGameObject(std::string _name) const;

	protected:
		virtual void Initialize();
		virtual void Update();
		virtual void Draw() const;
		virtual void End();

	private:
		std::list<GameObject*> pGameObjects_;  // �V�[���ɓo�ꂷ��Q�[���I�u�W�F�N�g
	};

	template<class GameObjectT, typename ...Args>
	inline GameObjectT* GameScene::Instantiate(Args... _args)
	{
		// ���N���X��GameObject�ł��邩
		static_assert(std::is_base_of<GameObject, GameObjectT>().value
			&& "GameObject�N���X���p�����Ă��Ȃ��N���X�̓C���X�^���X�ł��܂���B");

		GameObjectT* pInstance{ new GameObjectT{ _args... } };

		// TODO: �������A�������z��ɂ���
		pGameObjects_.push_back(pInstance);

		return pInstance;
	}
}
