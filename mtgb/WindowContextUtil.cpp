#include "WindowContextUtil.h"
#include "Game.h"
#include "WindowResource.h"
#include "Direct3DResource.h"
#include "ISystem.h"
#include "Screen.h"
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

void mtgb::WinCtxRes::SetFullscreen(bool _fullscreen, WindowContext _ctx)
{
	if (_fullscreen)
	{
		Game::System<WinCtxResManager>().Get<WindowResource>(_ctx).GetWindowInfo();
	}
	Game::System<WinCtxResManager>().Get<DXGIResource>(_ctx).SetFullscreen(_fullscreen);

	RECT monitorRect = WinCtxRes::Get<DXGIResource>(_ctx).GetAssignedMonitorRect();

	Game::System<WinCtxResManager>().Get<WindowResource>(_ctx).SetFullScreen(_fullscreen,monitorRect);

	UINT width, height;
	if (_fullscreen)
	{
		width = monitorRect.right - monitorRect.left; 
		height = monitorRect.bottom - monitorRect.top;
	}
	else
	{
		Vector2Int initialSize = Game::System<Screen>().GetInitialSize();
		width = static_cast<UINT>(initialSize.x);
		height = static_cast<UINT>(initialSize.y);
	}
	Game::System<WindowManager>().ResizeWindow(_ctx, width,height);
}

void WinCtxRes::ChangeResource(WindowContext ctx)
{
	Game::System<WinCtxResManager>().ChangeResource(ctx);
}