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
	//一つ目のウィンドウ
	WinCtxRes::ChangeResource(WindowContext::First);
	DirectX11Draw::Begin();
	DrawGameObjects(_scene);
	DirectX11Draw::End();

	//二つ目のウィンドウ
	WinCtxRes::ChangeResource(WindowContext::Second);
	DirectX11Draw::Begin();
	DrawGameObjects(_scene);
	DirectX11Draw::End();
}

void mtgb::RenderSystem::RenderImGuiWindows(GameScene& _scene)
{
	//ImGuiは一つ目のウィンドウに依存している
	WinCtxRes::ChangeResource(WindowContext::First);

	MTImGui& imGui = Game::System<MTImGui>();

	//RenderTargetViewをImGui用に切り替え
	imGui.SetImGuizmoRenderTargetView();



	DirectX11Draw::Begin();
	DrawGameObjects(_scene);

	imGui.BeginFrame();
	imGui.BeginImGuizmoFrame();


	//ゲーム画面表示
	bool isGuizmoActive = imGui.IsUsingImGuizmo() || imGui.IsOverImGuizmo();
	ImGuiWindowFlags flags = 0;
	if (isGuizmoActive) {
		flags |= ImGuiWindowFlags_NoMove; // ギズモ操作中はウィンドウ移動禁止
	}
	ImGui::Begin("Game View", 0, flags);

	imGui.RenderGameView();
	imGui.SetDrawList();
	ImGuiShowSystem::Instance().ShowAll(Show::GameView);

	imGui.End();

	//Inspector表示
	imGui.Begin("Inspector");
	ImGuiShowSystem::Instance().ShowAll(Show::Inspector);
	

	imGui.End();

	imGui.EndFrame();
	//DirectX11Draw::End();
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
