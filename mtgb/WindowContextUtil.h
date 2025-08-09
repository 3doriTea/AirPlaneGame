#pragma once
#include "WindowContextResourceManager.h"
#include "Game.h"
#include "ISystem.h"
namespace mtgb
{
	using WinCtxResManager = WindowContextResourceManager;

	/// <summary>
	/// WindowContextResource
	/// ラッパー、具象クラス依存の便利関数群
	/// </summary>
	namespace WinCtxRes
	{
		/// <summary>
		/// リソースを取得する
		/// </summary>
		/// <typeparam name="ResourceT">リソースの型</typeparam>
		/// <param name="ctx">取得するウィンドウの識別子</param>
		/// <returns></returns>
		template<typename ResourceT>
		ResourceT& Get(WindowContext ctx)
		{
			return Game::System<WinCtxResManager>().Get<ResourceT>(ctx);
		}

		/// <summary>
		/// リソースを切り替える
		/// </summary>
		/// <param name="ctx">切り替えるウィンドウの識別子</param>
		void ChangeResource(WindowContext ctx);

		/// <summary>
		/// 現在有効なウィンドウの識別子を返す
		/// </summary>
		/// <returns>現在のWindowContext</returns>
		WindowContext CurrContext();

		/// <summary>
		/// そのウィンドウのHWNDを取得する
		/// </summary>
		/// <param name="ctx">ウィンドウの識別子</param>
		/// <returns></returns>
		HWND GetHWND(WindowContext ctx);

		
	}
}