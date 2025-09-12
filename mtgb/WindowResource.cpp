#include "Game.h"
#include "ISystem.h"
#include "Input.h"
#include "WindowResource.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "WindowContextUtil.h"
#include "Direct3DResource.h"
#include "MTAssert.h"
#include "Screen.h"
using namespace mtgb;



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
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}


	switch (msg)
	{
	case WM_CLOSE:
		Game::Exit();
		return 0;
	case WM_DESTROY:  // ウィンドウを閉じた
		return S_OK;
	case WM_MOUSEMOVE:  // マウスが動いた
		Game::System<Input>().UpdateMousePositionData(LOWORD(lParam), HIWORD(lParam));
		return S_OK;
	case WM_SIZE:  // ウィンドウサイズが変わった
	{
		if (this && wParam != SIZE_MINIMIZED)
		{
			if (!isInitialized_)
			{
				// まだ初期化されていないならスキップする
				return S_OK;
			}

			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			// フルスクリーン切り替え時のリサイズ処理を有効化
			//Game::System<WindowManager>().ResizeWindow(context_, width, height);
		}
		return S_OK;
	}

	//case WM_NCCALCSIZE:
	//	// wParamについて：TRUEならNCCALCSIZE_PARAMS / FALSEならRect*
	//	if (wParam) return 0;
	//	break;
	//case WM_NCHITTEST:
	//{
	//	LRESULT hitResult{ DefWindowProc(hWnd, msg, wParam, lParam) };
	//	switch (hitResult)
	//	{
	//	case HTLEFT:
	//	case HTRIGHT:
	//	case HTTOP:
	//	case HTBOTTOM:
	//	case HTTOPLEFT:
	//	case HTTOPRIGHT:
	//	case HTBOTTOMLEFT:
	//	case HTBOTTOMRIGHT:
	//		return HTCLIENT; // サイズは変えさせない
	//	default:
	//		return hitResult;
	//	}
	//}
	default:  // それ以外のメッセージは譲渡
		break;
	}


	// NOTE: これが抜けているとウィンドウ表示されないし、エラーは出ないしで苦労する(した)
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND mtgb::WindowResource::GetHWND()
{
	return hWnd_;
}

void WindowResource::Initialize(WindowContext _windowContext)
{
	hWnd_ = Game::System<WindowManager>().CreateWindowContext(_windowContext);

	isActive_ = true;
	context_ = _windowContext;
	isInitialized_ = false;

	currInfo_.windowedStyle_ = GetWindowLong(hWnd_, GWL_STYLE);
	currInfo_.windowedExStyle_ = GetWindowLong(hWnd_, GWL_EXSTYLE);
	GetWindowRect(hWnd_, &currInfo_.windowedRect_);

	initialInfo_ = currInfo_;
}

void WindowResource::SetResource()
{

}

mtgb::WindowResource::WindowResource(const WindowResource& other)
	:WindowContextResource(other)
	,hWnd_(other.hWnd_)
	,isActive_(other.isActive_)
{

}

WindowResource* mtgb::WindowResource::Clone() const
{
	return new WindowResource(*this);
}

mtgb::WindowResource::WindowResource()
	: isInitialized_{false}
{
}

mtgb::WindowResource::~WindowResource()
{
	Release();
}

void mtgb::WindowResource::MarkInitialized()
{
	isInitialized_ = true;
}

void mtgb::WindowResource::OnResize(WindowContext _windowContext, UINT _width, UINT _height)
{
	if (_width == 0 || _height == 0) return;

	Game::System<Screen>().SetSize(static_cast<int>(_width), static_cast<int>(_height));

	//WindowConfigのサイズを更新
	WindowConfig config = Game::System<WindowManager>().GetWindowConfig(_windowContext);
	config.width = _width;
	config.height = _height;
	Game::System<WindowManager>().SetWindowConfig(_windowContext, config);

}

void mtgb::WindowResource::SetWindowMode()
{
	SetWindowModeImpl(currInfo_);
}


//void mtgb::WindowResource::SetInitialWindowMode()
//{
//	SetWindowModeImpl(initialInfo_);
//}

void mtgb::WindowResource::SetFullScreen(const RECT& _monitorRect)
{
	// フルスクリーンになる
	isFullscreen_ = true;

	GetWindowInfo();

	// ウィンドウスタイルを枠なしポップアップに変更
	SetWindowLong(hWnd_, GWL_STYLE, currInfo_.windowedStyle_ & ~(WS_CAPTION | WS_THICKFRAME));
	SetWindowLong(hWnd_, GWL_EXSTYLE, currInfo_.windowedExStyle_ & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

	// 指定されたモニター座標を直接使用
	SetWindowPos(hWnd_, HWND_TOP,
		_monitorRect.left, _monitorRect.top,// ウィンドウの位置
		_monitorRect.right - _monitorRect.left,// ウィンドウのサイズ(幅)
		_monitorRect.bottom - _monitorRect.top,// ウィンドウのサイズ(高さ)
		// オーナー(?)ウィンドウのZ順序は変更しない、スタイルの変更を適用
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

}



void mtgb::WindowResource::GetWindowInfo()
{
	// 現在のウィンドウのスタイルと位置を保存
	currInfo_.windowedStyle_ = GetWindowLong(hWnd_, GWL_STYLE);
	currInfo_.windowedExStyle_ = GetWindowLong(hWnd_, GWL_EXSTYLE);
	GetWindowRect(hWnd_, &currInfo_.windowedRect_);
}

void mtgb::WindowResource::SetPosition(const RECT& _monitorRect)
{
	SetWindowPos(hWnd_, HWND_NOTOPMOST,
		_monitorRect.left,
		_monitorRect.top,
		_monitorRect.right - _monitorRect.left,
		_monitorRect.bottom - _monitorRect.top,
		// オーナーウィンドウ(?)のZ順序は変更しない、スタイルの変更を適用
		SWP_NOOWNERZORDER);

	currInfo_.windowedRect_ = _monitorRect;
}

void mtgb::WindowResource::SetWindowModeImpl(WindowModeInfo _info)
{
	// ウィンドウモードに戻る
	isFullscreen_ = false;

	// ウィンドウスタイルを元に戻す
	SetWindowLong(hWnd_, GWL_STYLE, _info.windowedStyle_);
	SetWindowLong(hWnd_, GWL_EXSTYLE, _info.windowedExStyle_);

	// 保存しておいたウィンドウの位置、サイズを戻す
	RECT rect = _info.windowedRect_;
	SetWindowPos(hWnd_, HWND_NOTOPMOST,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		// オーナーウィンドウ(?)のZ順序は変更しない、スタイルの変更を適用
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

void mtgb::WindowResource::Release()
{
	DestroyWindow(hWnd_);
}


