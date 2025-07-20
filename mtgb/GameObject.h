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

		/// <summary>
		/// ���̃I�u�W�F�N�g���폜����
		/// </summary>
		inline void DestoryMe() { status_.toDestory_ = TRUE; }
		/// <summary>
		/// ���̃I�u�W�F�N�g�͍폜�\�肩
		/// </summary>
		/// <returns>�폜�\�� true / false</returns>
		inline const bool IsToDestory() const { return status_.toDestory_; }

	private:
		struct Status
		{
			uint8_t isActive : 1;
			uint8_t callUpdate_ : 1;
			uint8_t callDraw_ : 1;
			uint8_t toDestory_ : 1;  // �폜�\�肩
		} status_;


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
}
