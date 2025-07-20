#pragma once
#include "ISystem.h"
#include "GameScene.h"
#include "ReleaseUtility.h"

namespace mtgb
{
	class SceneSystem : public ISystem
	{
	public:
		SceneSystem();
		~SceneSystem();

		/// <summary>
		/// <para>�V�[���J�ڂ�����</para>
		/// <para>NOTE: �Ă񂾏u�ԁA�J�ڐ�N���X�̃R���X�g���N�^�͌Ă΂�邪�A</para>
		/// <para>----: Initialize�͎��̃t���[���ŌĂ΂��B</para>
		/// </summary>
		/// <typeparam name="NextSceneT">�J�ڐ�̃V�[���N���X�̌^</typeparam>
		/// <typeparam name="...Args">�J�ڐ�N���X�̃R���X�g���N�^�ϒ�����</typeparam>
		/// <param name="..._args">�J�ڐ�N���X�̃R���X�g���N�^����</param>
		template<class NextSceneT, typename ...Args>
		void Move(Args... _args);

		/// <summary>
		/// ���݂̃V�[���̃|�C���^�̎擾
		/// </summary>
		/// <returns>�V�[���̃|�C���^</returns>
		GameScene* GetActiveScene() const { return GameScene::pInstance_; }

		void Initialize() override;
		void Update() override;

	private:
		void ChangeScene();

	private:
		GameScene* pNextScene_;
	};

	template<class NextSceneT, typename ...Args>
	inline void SceneSystem::Move(Args... _args)
	{
		// ���N���X��GameScene�ł��邩
		static_assert(std::is_base_of<GameScene, NextSceneT>().value
			&& "GameScene�N���X���p�����Ă��Ȃ��N���X�ɃV�[���J�ڂł��܂���B");
		
		// ���Ɏ��̃V�[�������蓖�Ă��Ă���Ȃ�������
		SAFE_DELETE(pNextScene_);

		pNextScene_ = new NextSceneT{ _args... };
	}
}
