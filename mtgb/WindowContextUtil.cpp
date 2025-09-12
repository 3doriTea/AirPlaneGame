#include "WindowContextUtil.h"
#include "Game.h"
#include "WindowResource.h"
#include "Direct3DResource.h"
#include "ISystem.h"
#include "Screen.h"
#include "Direct2DResource.h"

using namespace mtgb;

/// <summary>
/// 現在有効なウィンドウの識別子を返す
/// </summary>
/// <returns>現在のWindowContext</returns>
WindowContext WinCtxRes::CurrContext()
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

void mtgb::WinCtxRes::SwapWindow(WindowContext _ctx1, WindowContext _ctx2)
{
	Game::System<WinCtxResManager>().SwapResource<WindowResource>(_ctx1, _ctx2);
	Game::System<WinCtxResManager>().SwapResource<DXGIResource>(_ctx1,_ctx2);
	Game::System<WinCtxResManager>().SwapResource<Direct3DResource>(_ctx1, _ctx2);
	Game::System<WinCtxResManager>().SwapResource<Direct2DResource>(_ctx1, _ctx2);
}





void WinCtxRes::ChangeResource(WindowContext ctx)
{
	Game::System<WinCtxResManager>().ChangeActiveResource(ctx);
}