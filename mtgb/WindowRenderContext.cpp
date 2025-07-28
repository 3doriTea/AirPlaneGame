#include "Game.h"
#include "Input.h"

#include "WindowRenderContext.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"

using namespace mtgb;

int WindowContext::outputCount{ 0 };

/// <summary>
/// ウィンドウからのメッセージを受信してImGuiの入力やイベントを有効にするためのコールバック関数
/// </summary>
/// <param name="hwnd">ウィンドウハンドル</param>
/// <param name="msg">メッセージ</param>
/// <param name="wParam">パラメータ</param>
/// <param name="lParam">パラメータ</param>
/// <returns></returns>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// <summary>
/// 各ウィンドウに共通の処理を記述。これをWNDCLASSに渡す
/// </summary>
/// <param name="hWnd"></param>
/// <param name="msg"></param>
/// <param name="wParam"></param>
/// <param name="lParam">インスタンスを取得するために、CreateWindowのlpParamにthisを渡す必要がある</param>
/// <returns></returns>
LRESULT WindowContext::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WindowContext* pThis;
	if (msg == WM_NCCREATE)
	{
		//作成時のパラメータからthisを取得、キャスト
		pThis = static_cast<WindowContext*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		//thisのUSERDATAにthisを紐付ける
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->hWnd_ = hWnd;
	}
	else
	{
		//hWndにthisを紐づけておいたので取得
		pThis = reinterpret_cast<WindowContext*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}
	if (pThis)
	{
		return pThis->HandleWindowMessage(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);

}

LRESULT WindowContext::HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

WindowContext::WindowContext()  
   : hWnd_(nullptr),  
     pPeekedMessage_(nullptr),  
     pSwapChain_(nullptr),  
     pOutput_(nullptr),  
     pRenderTargetView_(nullptr),  
     pDepthStencil_(nullptr),  
     pDepthStencilView_(nullptr),  
     isActive_(false),  
     outputIndex_(0),  
     viewport_({0}),  
     windowClass_({0}),  
     windowRect_({0})  
{  
}

void WindowContext::Initialize(HWND windowHandle)
{

}


