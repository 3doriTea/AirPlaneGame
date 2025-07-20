#include "SceneSystem.h"
#include "GameObject.h"

mtgb::SceneSystem::SceneSystem() :
	pNextScene_{ nullptr }
{
}

mtgb::SceneSystem::~SceneSystem()
{
}

void mtgb::SceneSystem::Initialize()
{
}

void mtgb::SceneSystem::Update()
{
	// ���̃V�[�����p�ӂ���Ă���Ȃ�V�[���`�F���W����
	if (pNextScene_)
	{
		ChangeScene();
	}

	if (GameScene::pInstance_ == nullptr)
	{
		return;  // �V�[�����Ȃ��Ȃ��A
	}

	// ���݂̃V�[��
	GameScene& currentScene{ *GameScene::pInstance_ };

	// �X�V����
	currentScene.Update();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Update();
	}

	// �`�揈��
	currentScene.Draw();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Draw();
	}

	// �폜����
	for (auto&& itr = currentScene.pGameObjects_.begin();
		itr != currentScene.pGameObjects_.end();)
	{
		if ((*itr)->IsToDestory())
		{
			Game::RemoveEntityComponent((*itr)->GetEntityId());
			itr = currentScene.pGameObjects_.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void mtgb::SceneSystem::ChangeScene()
{
	// �������݂̃V�[��������Ȃ�I������
	if (GameScene::pInstance_)
	{
		GameScene::pInstance_->End();
	}

	// ������ă|�C���^�ύX
	SAFE_DELETE(GameScene::pInstance_);
	GameScene::pInstance_ = pNextScene_;
	pNextScene_ = nullptr;

	// �`�F���W�����V�[���̏���������
	GameScene::pInstance_->Initialize();
}
