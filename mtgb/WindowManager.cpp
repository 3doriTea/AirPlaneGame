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
	(*ppContext)->windowClass_.cbSize = sizeof(WNDCLASSEXW);                 // �\���̂̃T�C�Y
	(*ppContext)->windowClass_.hInstance = GetModuleHandle(NULL);           // �C���X�^���X�n���h��
	//(*ppContext)->windowClass_.lpszClassName = szWindowClass;        // �E�B���h�E�N���X��
	(*ppContext)->windowClass_.lpszClassName = (*ppContext)->windowClassName_.c_str();        // �E�B���h�E�N���X��
	(*ppContext)->windowClass_.lpfnWndProc =  WindowContext::WndProc;                       // ���b�Z�[�W���󂯎��R�[���o�b�N�֐�
	(*ppContext)->windowClass_.style = CS_VREDRAW | CS_HREDRAW;             // �X�^�C��
	(*ppContext)->windowClass_.hIcon = LoadIcon(nullptr, IDI_APPLICATION);  // �A�C�R��
	(*ppContext)->windowClass_.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);    // �������A�C�R��
	(*ppContext)->windowClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);    // �}�E�X�J�[�\��
	(*ppContext)->windowClass_.lpszMenuName = nullptr;                      // ���j���[
	(*ppContext)->windowClass_.cbClsExtra = 0;
	(*ppContext)->windowClass_.cbWndExtra = 0;
	(*ppContext)->windowClass_.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // �w�i�F ��

	massert(RegisterClassEx(&(*ppContext)->windowClass_) != 0  // �E�B���h�E�N���X�̓o�^�ɐ���
		&& "RegisterClassExW�Ɏ��s @WindowManager::CreateWindowRenderContext");

	RECT windowRect{ 0, 0, config.width, config.height };
	massert(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW) != FALSE  // �X�N���[���{�[�_���l�������E�B���h�E�T�C�Y�̎擾�ɐ���
		&& "AdjustWindowRectEx�Ɏ��s @WindowManager::CreateWindowRenderContext");
	
	

	//WCHAR

	{  // window�̍쐬
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
		massert(hInstance != NULL  // ���W���[���n���h���̎擾�ɐ���
			&& "���W���[���n���h���̎擾�Ɏ��s");
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

		massert((*ppContext)->hWnd_ != NULL  // �E�B���h�E�̍쐬�ɐ������Ă���
			&& "�E�B���h�E�̍쐬�Ɏ��s");

		massert(IsWindow((*ppContext)->hWnd_)  // �E�B���h�E�n���h�����������쐬����Ă���
			&& "Window�ł͂Ȃ��n���h��������Ă��܂���");

		massert(SetWindowText((*ppContext)->hWnd_, (*ppContext)->windowTitle_.c_str())
			&& "SetWindowTextW�Ɏ��s");

		// NOTE: ShowWindow�̖߂�l�ɒ���
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-showwindow
		ShowWindow((*ppContext)->hWnd_, SW_SHOW);  // �E�B���h�E��\��

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
