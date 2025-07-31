#include "SceneSystem.h"
#include "GameObject.h"
#include "DoubleWindow.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
#include "Input.h"

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

	Game::System<DoubleWindow>().ChangeFirstWindow();
	Game::System<Input>().Update();

	Game::System<DoubleWindow>().ChangeSecondWindow();
	Game::System<Input>().Update();

	// ���݂̃V�[��
	GameScene& currentScene{ *GameScene::pInstance_ };

	// �X�V����
	currentScene.Update();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Update();
	}

	// �`�揈��


	Game::System<DoubleWindow>().ChangeFirstWindow();
	DirectX11Draw::Begin();
	currentScene.Draw();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Draw();
	}
	DirectX11Draw::End();

	Game::System<DoubleWindow>().ChangeSecondWindow();
	DirectX11Draw::Begin();
	currentScene.Draw();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Draw();
	}
	DirectX11Draw::End();

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
