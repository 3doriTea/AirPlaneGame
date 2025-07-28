#include "WindowManager.h"
#include "WindowRenderContext.h"
#include <Windows.h>
#include "MTAssert.h"
#include "MTStringUtility.h"

namespace
{
	
}

MSG* mtgb::WindowManager::pPeekedMessage_{ nullptr };

mtgb::WindowManager::WindowManager() 
{
	pPeekedMessage_ = new MSG{};
}
mtgb::WindowManager::~WindowManager()
{
	delete pPeekedMessage_;
}
void mtgb::WindowManager::CreateWindowRenderContext(const WindowConfig& config, WindowContext ** ppContext)
{
	*ppContext = new WindowContext();
	(*ppContext)->windowTitle_ = config.title;
	(*ppContext)->windowClassName_ = config.className;
	//(*ppContext)->windowClass_ = windowClass;

	//WNDCLASSEXW windowClass{};
	(*ppContext)->windowClass_.cbSize = sizeof(WNDCLASSEXW);                 // 構造体のサイズ
	(*ppContext)->windowClass_.hInstance = GetModuleHandle(NULL);           // インスタンスハンドル
	//(*ppContext)->windowClass_.lpszClassName = szWindowClass;        // ウィンドウクラス名
	(*ppContext)->windowClass_.lpszClassName = (*ppContext)->windowClassName_.c_str();        // ウィンドウクラス名
	(*ppContext)->windowClass_.lpfnWndProc =  WindowContext::WndProc;                       // メッセージを受け取るコールバック関数
	(*ppContext)->windowClass_.style = CS_VREDRAW | CS_HREDRAW;             // スタイル
	(*ppContext)->windowClass_.hIcon = LoadIcon(nullptr, IDI_APPLICATION);  // アイコン
	(*ppContext)->windowClass_.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);    // 小さいアイコン
	(*ppContext)->windowClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);    // マウスカーソル
	(*ppContext)->windowClass_.lpszMenuName = nullptr;                      // メニュー
	(*ppContext)->windowClass_.cbClsExtra = 0;
	(*ppContext)->windowClass_.cbWndExtra = 0;
	(*ppContext)->windowClass_.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // 背景色 白

	massert(RegisterClassEx(&(*ppContext)->windowClass_) != 0  // ウィンドウクラスの登録に成功
		&& "RegisterClassExWに失敗 @WindowManager::CreateWindowRenderContext");

	RECT windowRect{ 0, 0, config.width, config.height };
	massert(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW) != FALSE  // スクリーンボーダを考慮したウィンドウサイズの取得に成功
		&& "AdjustWindowRectExに失敗 @WindowManager::CreateWindowRenderContext");
	
	

	//WCHAR

	{  // windowの作成
		DWORD     exWindowStyle{ 0 };
		LPCSTR    className{ (*ppContext)->windowClassName_.c_str() };
		LPCSTR    windowName{ (*ppContext)->windowTitle_.c_str() };
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


		(*ppContext)->hWnd_ = CreateWindowEx(
			exWindowStyle,
			(*ppContext)->windowClassName_.c_str(),
			(*ppContext)->windowTitle_.c_str(),
			windowStyle,
			windowPositionX,
			windowPositionY,
			windowWidth,
			windowHeight,
			hWndParent,
			hMenu,
			hInstance,
			param);

		massert((*ppContext)->hWnd_ != NULL  // ウィンドウの作成に成功している
			&& "ウィンドウの作成に失敗");

		massert(IsWindow((*ppContext)->hWnd_)  // ウィンドウハンドルが正しく作成されている
			&& "Windowではないハンドルが作られてしまった");

		massert(SetWindowText((*ppContext)->hWnd_, (*ppContext)->windowTitle_.c_str())
			&& "SetWindowTextWに失敗");

		// NOTE: ShowWindowの戻り値に注意
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-showwindow
		ShowWindow((*ppContext)->hWnd_, SW_SHOW);  // ウィンドウを表示

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
