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
	windowClass.cbSize = sizeof(WNDCLASSEX);                 // �\���̂̃T�C�Y
	windowClass.hInstance = GetModuleHandle(NULL);           // �C���X�^���X�n���h��
	windowClass.lpszClassName = config.title.c_str();        // �E�B���h�E�N���X��
	windowClass.lpfnWndProc =  WindowRenderContext::WndProc;                       // ���b�Z�[�W���󂯎��R�[���o�b�N�֐�
	windowClass.style = CS_VREDRAW | CS_HREDRAW;             // �X�^�C��
	windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);  // �A�C�R��
	windowClass.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);    // �������A�C�R��
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);    // �}�E�X�J�[�\��
	windowClass.lpszMenuName = nullptr;                      // ���j���[
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // �w�i�F ��

	massert(RegisterClassExW(&windowClass) != 0  // �E�B���h�E�N���X�̓o�^�ɐ���
		&& "RegisterClassExW�Ɏ��s @WindowManager::CreateWindowRenderContext");

	RECT windowRect{ 0, 0, config.width, config.height };
	massert(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW) != FALSE  // �X�N���[���{�[�_���l�������E�B���h�E�T�C�Y�̎擾�ɐ���
		&& "AdjustWindowRectEx�Ɏ��s @WindowManager::CreateWindowRenderContext");

	*ppContext = new WindowRenderContext();


	{  // window�̍쐬
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
		massert(hInstance != NULL  // ���W���[���n���h���̎擾�ɐ���
			&& "���W���[���n���h���̎擾�Ɏ��s");
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

		massert(hWnd != NULL  // �E�B���h�E�̍쐬�ɐ������Ă���
			&& "�E�B���h�E�̍쐬�Ɏ��s");

		massert(IsWindow(hWnd)  // �E�B���h�E�n���h�����������쐬����Ă���
			&& "Window�ł͂Ȃ��n���h��������Ă��܂���");

		// NOTE: ShowWindow�̖߂�l�ɒ���
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-showwindow
		ShowWindow(hWnd, SW_SHOWDEFAULT);  // �E�B���h�E��\��
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
	// MEMO: ��2������ nullptr ���w�肵�Ă��邽�߁A
	//     : ���C���E�B���h�E�Ɍ��炸�A�X���b�h�ɑ�����S�Ă�MSG�擾
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-peekmessagea
	if (PeekMessage(
		pPeekedMessage_,
		nullptr,
		0u,  // �t�B���^���� (�ŏ��̃��b�Z�[�W)
		0u,  // �t�B���^���� (�Ō�̃��b�Z�[�W)
		PM_REMOVE))  // �s�[�N�������Ƃ̑S�ăf�L���[
	{
		TranslateMessage(pPeekedMessage_);  // ���z�L�[���͂𕶎����̓C�x���g�Ƃ���enqueue
		DispatchMessage(pPeekedMessage_);  // enqueue�������b�Z�[�W�����ۂɏ���������
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
