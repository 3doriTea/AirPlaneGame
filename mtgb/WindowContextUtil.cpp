#include "WindowContextUtil.h"

using namespace mtgb;

/// <summary>
/// 現在有効なウィンドウの識別子を返す
/// </summary>
/// <returns>現在のWindowContext</returns>
WindowContext WinCtxRes::Current()
{
	return Game::System<WinCtxResManager>().CurrCtx();
}

/// <summary>
/// そのウィンドウのHWNDを取得する
/// </summary>
/// <param name="ctx">ウィンドウの識別子</param>
/// <returns></returns>
HWND WinCtxRes::GetHWND(WindowContext ctx)
{
	return WinCtxRes::Get<WindowResource>(ctx).GetHWND();
}

void WinCtxRes::ChangeResource(WindowContext ctx)
{
	Game::System<WinCtxResManager>().ChangeResource(ctx);
}