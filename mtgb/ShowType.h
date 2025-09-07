#pragma once
/// <summary>
	/// 表示するImGuiWindow
	/// </summary>
enum class ShowType
{
	Inspector, // プロパティを表示
	SceneView, // デバッグ用のゲーム画面
	Settings, // ゲームの設定をいじる画面
	None
};