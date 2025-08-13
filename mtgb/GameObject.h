#pragma once
#include "Entity.h"
#include <string>
#include "GameObjectBuilder.h"
#include <bitset>
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
		/// �R���|�[�l���g���擾/�ǉ�
		/// </summary>
		/// <typeparam name="ComponentT">�R���|�[�l���g�̌^</typeparam>
		/// <returns>�R���|�[�l���g�̃|�C���^</returns>
		template<class ComponentT>
		ComponentT* Component();

		/// <summary>
		/// �Q�[���I�u�W�F�N�g���C���X�^���X����
		/// </summary>
		/// <typeparam name="GameObjectT">�Q�[���I�u�W�F�N�g�̌^</typeparam>
		/// <typeparam name="...Args">�R���X�g���N�^�ϒ������^</typeparam>
		/// <param name="..._args">�R���X�g���N�^�ϒ�����</param>
		/// <returns>�C���X�^���X���ꂽ�Q�[���I�u�W�F�N�g�̃|�C���^</returns>
		template<class GameObjectT, typename ...Args>
		GameObjectT* Instantiate(Args... _args);

		template<typename GameSceneT>
		GameSceneT& GetScene();

		GameObject* FindGameObject(const std::string& _name);
		GameObject* FindGameObject(const EntityId _entityId);

		/// <summary>
		/// ���̃I�u�W�F�N�g���폜����
		/// </summary>
		inline void DestroyMe() { status_.toDestroy_ = TRUE; }
		/// <summary>
		/// ���̃I�u�W�F�N�g�͍폜�\�肩
		/// </summary>
		/// <returns>�폜�\�� true / false</returns>
		inline const bool IsToDestroy() const { return status_.toDestroy_; }
		std::string GetName() { return name_; }
		/// <summary>
		/// ���C���[�t���O���擾
		/// </summary>
		/// <returns>���C���[�t���O���擾����</returns>
		GameObjectLayerFlag GetLayerFlag() const { return layerFlag_; }

	private:

		std::string name_;

		struct Status
		{
			uint8_t isActive_ : 1;
			uint8_t callUpdate_ : 1;
			uint8_t callDraw_ : 1;
			uint8_t toDestroy_ : 1;  // �폜�\�肩
		} status_;

		GameObjectLayerFlag layerFlag_;  // ���C���[�̃t���O


		std::bitset<COMPONENT_CAPACITY> componentsFlag_;  // �R���|�[�l���g�̃t���O
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
			&& "�A�N�e�B�u�V�[���̎擾�Ɏ��s @GameObject::GetScene");

		return *pActiveScene;
	}
}
