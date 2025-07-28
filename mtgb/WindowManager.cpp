#include "WindowManager.h"
#include "WindowRenderContext.h"
#include <Windows.h>
#include "MTAssert.h"

MSG* mtgb::WindowManager::pPeekedMessage_{ nullptr };

mtgb::WindowManager::WindowManager() 
{
	pPeekedMessage_ = new MSG{};
}
mtgb::WindowManager::~WindowManager()
{
	delete pPeekedMessage_;
}
void mtgb::WindowManager::CreateWindowRenderContext(const WindowConfig& config, WindowRenderContext ** ppContext)
{
	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEX);                 // 構造体のサイズ
	windowClass.hInstance = GetModuleHandle(NULL);           // インスタンスハンドル
	windowClass.lpszClassName = config.title.c_str();        // ウィンドウクラス名
	windowClass.lpfnWndProc =  WindowRenderContext::WndProc;                       // メッセージを受け取るコールバック関数
	windowClass.style = CS_VREDRAW | CS_HREDRAW;             // スタイル
	windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);  // アイコン
	windowClass.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);    // 小さいアイコン
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);    // マウスカーソル
	windowClass.lpszMenuName = nullptr;                      // メニュー
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // 背景色 白

	massert(RegisterClassExW(&windowClass) != 0  // ウィンドウクラスの登録に成功
		&& "RegisterClassExWに失敗 @WindowManager::CreateWindowRenderContext");

	RECT windowRect{ 0, 0, config.width, config.height };
	massert(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW) != FALSE  // スクリーンボーダを考慮したウィンドウサイズの取得に成功
		&& "AdjustWindowRectExに失敗 @WindowManager::CreateWindowRenderContext");

	*ppContext = new WindowRenderContext();


	{  // windowの作成
		DWORD     exWindowStyle{ WS_EX_OVERLAPPEDWINDOW };
		LPCWSTR    className{ config.title.c_str() };
		LPCWSTR    windowName{ config.title.c_str() };
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
		LPVOID    param{ *ppContext };


		HWND hWnd = CreateWindowExW(
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

		massert(hWnd != NULL  // ウィンドウの作成に成功している
			&& "ウィンドウの作成に失敗");

		massert(IsWindow(hWnd)  // ウィンドウハンドルが正しく作成されている
			&& "Windowではないハンドルが作られてしまった");

		// NOTE: ShowWindowの戻り値に注意
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-showwindow
		ShowWindow(hWnd, SW_SHOWDEFAULT);  // ウィンドウを表示
		auto tmp = *ppContext;
		(*ppContext)->hWnd_ = hWnd;
		(*ppContext)->windowClass_ = windowClass;
		(*ppContext)->windowTitle_ = config.title;
		(*ppContext)->windowRect_ = windowRect;
	}
}

void mtgb::WindowManager::Initialize()
{
}

void mtgb::WindowManager::Update()
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

//const HWND mtgb::WindowManager::GetHWND()
//{
//	return 
//}

//void mtgb::WindowManager::GenerateWndClassEx(const WindowConfig& config, WNDCLASSEX* _pWndClassEx)
//{
//
//}
