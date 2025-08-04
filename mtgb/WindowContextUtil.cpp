#include "WindowContextUtil.h"

using namespace mtgb;

/// <summary>
/// ���ݗL���ȃE�B���h�E�̎��ʎq��Ԃ�
/// </summary>
/// <returns>���݂�WindowContext</returns>
WindowContext WinCtxRes::Current()
{
	return Game::System<WinCtxResManager>().CurrCtx();
}

/// <summary>
/// ���̃E�B���h�E��HWND���擾����
/// </summary>
/// <param name="ctx">�E�B���h�E�̎��ʎq</param>
/// <returns></returns>
HWND WinCtxRes::GetHWND(WindowContext ctx)
{
	return WinCtxRes::Get<WindowResource>(ctx).GetHWND();
}

void WinCtxRes::ChangeResource(WindowContext ctx)
{
	Game::System<WinCtxResManager>().ChangeResource(ctx);
}