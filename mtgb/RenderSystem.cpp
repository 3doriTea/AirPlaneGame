#include "RenderSystem.h"
#include "WindowContextUtil.h"
#include "DirectX11Draw.h"
#include "ImGuiRenderer.h"
#include "ImGuiShowable.h"
#include "SceneSystem.h"
#include "GameObject.h"
#include "Debug.h"
#include "MTImGui.h"
#include "../ImGui/imgui.h"

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
	DrawGameObjects(_scene, [](GameObject* pGameObject) { return pGameObject->GetLayerFlag().Has(GameObjectLayer::A); });
	DirectX11Draw::End();

	//二つ目のウィンドウ
	WinCtxRes::ChangeResource(WindowContext::Second);
	DirectX11Draw::Begin();
	DrawGameObjects(_scene, [](GameObject* pGameObject) { return pGameObject->GetLayerFlag().Has(GameObjectLayer::B); });
	DirectX11Draw::End();
}

void mtgb::RenderSystem::RenderImGuiWindows(GameScene& _scene)
{
	using mtbit::operator|;

	// ImGuiは一つ目のウィンドウに依存している
	WinCtxRes::ChangeResource(WindowContext::First);

	ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();

	// RenderTargetViewをImGui用に切り替え
	imGui.SetImGuizmoRenderTargetView();

	DirectX11Draw::Begin();
	imGui.SetGameViewCamera();
	DrawGameObjects(_scene, [](GameObject* pGameObject) { return pGameObject->GetLayerFlag().Has(GameObjectLayer::A | GameObjectLayer::B); });

	imGui.BeginFrame();
	imGui.BeginImGuizmoFrame();

	// SceneView表示
	imGui.Begin(MTImGui::GetName(ShowType::SceneView).data(), ImGuiRenderer::WindowFlag::NoMoveWhenHovered);
	imGui.UpdateCamera(MTImGui::GetName(ShowType::SceneView).data());
	imGui.RenderSceneView();
	imGui.SetDrawList();
	MTImGui::Instance().ShowAll(ShowType::SceneView);
	imGui.End();

	// Inspector表示
	imGui.Begin(MTImGui::GetName(ShowType::Inspector).data());
	MTImGui::Instance().ShowAll(ShowType::Inspector);
	imGui.End();

	// ログ表示
	imGui.Begin(Debug::GetName().data());

	using mtgb::Debug;
	const std::list<mtgb::LogEntry>& logs = Game::System<Debug>().GetLog();

	static int selectedLog = -1;
	int idx = 0;
	for (const mtgb::LogEntry log : logs)
	{
		std::string text = log.msg + " (" + std::to_string(log.count) + ")";

		if (ImGui::Selectable(text.c_str(), selectedLog == idx))
		{
			selectedLog = idx;
		}
		++idx;
	}

	// ログの詳細表示
	if (selectedLog >= 0)
	{
		auto it = logs.begin();
		std::advance(it, selectedLog);

		ImGui::Begin("Log Details");
		ImGui::Text("File: %s", it->file.c_str());
		ImGui::Text("Line: %d", it->line);
		ImGui::Text("Function: %s", it->func.c_str());
		ImGui::End();
	}

	imGui.End();

	imGui.EndFrame();

	//DirectX11Draw::End();
}

void mtgb::RenderSystem::RenderGameView(GameScene& _scene)
{
	ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();

}

void mtgb::RenderSystem::DrawGameObjects(GameScene& _scene, const std::function<bool(GameObject*)> _isDrawTargetCallback)
{
	_scene.Draw();
	for (auto&& gameObject : _scene.pGameObjects_)
	{
		if (_isDrawTargetCallback(gameObject))
		{
			gameObject->Draw();
		}
	}
}