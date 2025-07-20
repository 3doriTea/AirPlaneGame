#include "MainWindow.h"
#include <Windows.h>
#include "Game.h"
#include "MTAssert.h"
#include "Screen.h"
#include "Input.h"

using namespace mtgb;

/// <summary>
/// ウィンドウからのメッセージを受信するコールバック関数
/// </summary>
/// <param name="hWnd">ウィンドウハンドル</param>
/// <param name="message">メッセージ</param>
/// <param name="wParam">パラメータ</param>
/// <param name="lParam">パラメータ</param>
/// <returns>結果</returns>
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

mtgb::MainWindow::MainWindow() :
	hWnd_{ nullptr },
	pPeekedMessage_{ nullptr }
{
	pPeekedMessage_ = new MSG{};
}

mtgb::MainWindow::~MainWindow()
{
	delete pPeekedMessage_;
}

void mtgb::MainWindow::Initialize()
{
	WNDCLASSEX windowClass{};  // ウィンドウの設定
	GenerateWndClassEx(&windowClass);

	massert(RegisterClassEx(&windowClass) != 0  // ウィンドウクラスの登録に成功
		&& "ウィンドウクラスの登録に失敗");

	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	RECT windowRect{ 0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y };
	massert(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW) != FALSE  // スクリーンボーダを考慮したウィンドウサイズの取得に成功
		&& "スクリーンボーダを考慮したウィンドウサイズの取得に失敗");

	{  // windowの作成
		DWORD     exWindowStyle{ WS_EX_OVERLAPPEDWINDOW };
		LPCSTR    className{ Game::Title().data() };
		LPCSTR    windowName{ Game::Title().data() };
		DWORD     windowStyle{ WS_OVERLAPPEDWINDOW };//& ~WS_THICKFRAME };
		int       windowPositionX{ CW_USEDEFAULT };
		int       windowPositionY{ CW_USEDEFAULT };
		int       windowWidth{ windowRect.right - windowRect.left };
		int       windowHeight{ windowRect.bottom - windowRect.top };
		HWND      hWndParent{ nullptr };
		HMENU     hMenu{ nullptr };
		HINSTANCE hInstance{ GetModuleHandle(NULL) };
		massert(hInstance != NULL  // モジュールハンドルの取得に成功
			&& "モジュールハンドルの取得に失敗");
		LPVOID    param{ nullptr };

		hWnd_ = CreateWindowEx(
			exWindowStyle,
			className,
			windowName,
			windowStyle,
			windowPositionX,
			windowPositionY,
			windowWidth,
			windowHeight,
			hWndParent,
			hMenu,
			hInstance,
			param);
	}

	massert(hWnd_ != NULL  // ウィンドウの作成に成功している
		&& "ウィンドウの作成に失敗");

	massert(IsWindow(hWnd_)  // ウィンドウハンドルが正しく作成されている
		&& "Windowではないハンドルが作られてしまった");

	// NOTE: ShowWindowの戻り値に注意
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-showwindow
	ShowWindow(hWnd_, SW_SHOWDEFAULT);  // ウィンドウを表示
}

void mtgb::MainWindow::Update()
{
	// MEMO: 第2引数で nullptr を指定しているため、
	//     : メインウィンドウに限らず、スレッドに属する全てのMSG取得
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-peekmessagea
	if (PeekMessage(
		pPeekedMessage_,
		nullptr,
		0u,  // フィルタ無し (最初のメッセージ)
		0u,  // フィルタ無し (最後のメッセージ)
		PM_REMOVE))  // ピークしたあとの全てデキュー
	{
		TranslateMessage(pPeekedMessage_);  // 仮想キー入力を文字入力イベントとしてenqueue
		DispatchMessage(pPeekedMessage_);  // enqueueしたメッセージを実際に処理させる
		return;
	}
}

void mtgb::MainWindow::GenerateWndClassEx(WNDCLASSEX* _pWndClassEx)
{
	_pWndClassEx->cbSize = sizeof(WNDCLASSEX);                 // 構造体のサイズ
	_pWndClassEx->hInstance = GetModuleHandle(NULL);           // インスタンスハンドル
	_pWndClassEx->lpszClassName = Game::Title().data();        // ウィンドウクラス名
	_pWndClassEx->lpfnWndProc = WndProc;                       // メッセージを受け取るコールバック関数
	_pWndClassEx->style = CS_VREDRAW | CS_HREDRAW;             // スタイル
	_pWndClassEx->hIcon = LoadIcon(nullptr, IDI_APPLICATION);  // アイコン
	_pWndClassEx->hIconSm = LoadIcon(nullptr, IDI_WINLOGO);    // 小さいアイコン
	_pWndClassEx->hCursor = LoadCursor(nullptr, IDC_ARROW);    // マウスカーソル
	_pWndClassEx->lpszMenuName = nullptr;                      // メニュー
	_pWndClassEx->cbClsExtra = 0;
	_pWndClassEx->cbWndExtra = 0;
	_pWndClassEx->hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // 背景色 白
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
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
		LRESULT hitResult{ DefWindowProc(hWnd, message, wParam, lParam) };
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

	/*if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}*/

	// NOTE: これが抜けているとウィンドウ表示されないし、エラーは出ないしで苦労する(した)
	return DefWindowProc(hWnd, message, wParam, lParam);
}
