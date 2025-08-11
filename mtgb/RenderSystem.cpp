#include "RenderSystem.h"
#include "WindowContextUtil.h"
#include "DirectX11Draw.h"
#include "MTImGui.h"
#include "ImGuiShowable.h"
#include "SceneSystem.h"
#include "GameObject.h"

void mtgb::RenderSystem::Initialize()
{
}

void mtgb::RenderSystem::Update()
{
}

void mtgb::RenderSystem::Render(GameScene& _scene)
{
	RenderDirectXWindows(_scene);

	RenderImGuiWindows(_scene);
}

void mtgb::RenderSystem::RenderDirectXWindows(GameScene& _scene)
{
	//��ڂ̃E�B���h�E
	WinCtxRes::ChangeResource(WindowContext::First);
	DirectX11Draw::Begin();
	DrawGameObjects(_scene);
	DirectX11Draw::End();

	//��ڂ̃E�B���h�E
	WinCtxRes::ChangeResource(WindowContext::Second);
	DirectX11Draw::Begin();
	DrawGameObjects(_scene);
	DirectX11Draw::End();
}

void mtgb::RenderSystem::RenderImGuiWindows(GameScene& _scene)
{
	//ImGui�͈�ڂ̃E�B���h�E�Ɉˑ����Ă���
	WinCtxRes::ChangeResource(WindowContext::First);

	MTImGui& imGui = Game::System<MTImGui>();

	//RenderTargetView��ImGui�p�ɐ؂�ւ�
	imGui.SetImGuizmoRenderTargetView();



	DirectX11Draw::Begin();
	DrawGameObjects(_scene);

	imGui.BeginFrame();
	imGui.BeginImGuizmoFrame();


	//�Q�[����ʕ\��
	imGui.Begin("Game View");

	imGui.RenderGameView();
	imGui.SetDrawList();
	ImGuiShowSystem::Instance().ShowAll(Show::GameView);

	imGui.End();

	//Inspector�\��
	imGui.Begin("Inspector");
	ImGuiShowSystem::Instance().ShowAll(Show::Inspector);
	imGui.End();

	imGui.EndFrame();
	DirectX11Draw::End();
}

void mtgb::RenderSystem::RenderGameView(GameScene& _scene)
{
	MTImGui& imGui = Game::System<MTImGui>();

}

void mtgb::RenderSystem::DrawGameObjects(GameScene& _scene)
{
	_scene.Draw();
	for (auto&& gameObject : _scene.pGameObjects_)
	{
		gameObject->Draw();
	}
}
