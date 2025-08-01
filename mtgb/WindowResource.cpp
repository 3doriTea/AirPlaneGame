#include "Game.h"
#include "ISystem.h"
#include "Input.h"
#include "WindowResource.h"

using namespace mtgb;
int mtgb::WindowResource::outputCount = 0;

/// <summary>
/// 各ウィンドウに共通の処理を記述。これをWNDCLASSに渡す
/// </summary>
/// <param name="hWnd"></param>
/// <param name="msg"></param>
/// <param name="wParam"></param>
/// <param name="lParam">インスタンスを取得するために、CreateWindowのlpParamにthisを渡す必要がある</param>
/// <returns></returns>
LRESULT WindowResource::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WindowResource* pThis;
	if (msg == WM_NCCREATE)
	{
		//作成時のパラメータからthisを取得、キャスト
		pThis = static_cast<WindowResource*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		//thisのUSERDATAにthisを紐付ける
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->hWnd_ = hWnd;
	}
	else
	{
		//hWndにthisを紐づけておいたので取得
		pThis = reinterpret_cast<WindowResource*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (pThis)
		{
			pThis->hWnd_ = hWnd;
		}
	}
	if (pThis)
	{
		return pThis->HandleWindowMessage(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);

}

LRESULT WindowResource::HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_DESTROY:  // ウィンドウを閉じた
		Game::Exit();
		return S_OK;
	case WM_MOUSEMOVE:  // マウスが動いた
		Game::System<Input>().UpdateMousePositionData(LOWORD(lParam), HIWORD(lParam));
		return S_OK;
	case WM_SIZE:  // ウィンドウサイズが変わった

		return S_OK;
	case WM_NCCALCSIZE:
		// wParamについて：TRUEならNCCALCSIZE_PARAMS / FALSEならRect*
		if (wParam) return 0;
		break;
	case WM_NCHITTEST:
	{
		LRESULT hitResult{ DefWindowProc(hWnd, msg, wParam, lParam) };
		switch (hitResult)
		{
		case HTLEFT:
		case HTRIGHT:
		case HTTOP:
		case HTBOTTOM:
		case HTTOPLEFT:
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
		case HTBOTTOMRIGHT:
			return HTCLIENT; // サイズは変えさせない
		default:
			return hitResult;
		}
	}
	default:  // それ以外のメッセージは譲渡
		break;
	}


	// NOTE: これが抜けているとウィンドウ表示されないし、エラーは出ないしで苦労する(した)
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void WindowResource::Initialize(WindowContext _windowContext)
{
	hWnd_ = Game::System<WindowManager>().CreateWindowContext(_windowContext);

	isActive_ = true;
	outputIndex_ = outputCount++;
}

void WindowResource::SetResource()
{

}

mtgb::WindowResource::WindowResource(const WindowResource& other)
	:WindowContextResource(other)
	,hWnd_(other.hWnd_)
	,isActive_(other.isActive_)
	,outputIndex_(other.outputIndex_)
{

}

WindowResource* mtgb::WindowResource::Clone() const
{
	return new WindowResource(*this);
}

mtgb::WindowResource::WindowResource()
{
}


