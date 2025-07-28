#include "Game.h"
#include "Input.h"

#include "WindowRenderContext.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"

using namespace mtgb;

int WindowContext::outputCount{ 0 };

/// <summary>
/// �E�B���h�E����̃��b�Z�[�W����M����ImGui�̓��͂�C�x���g��L���ɂ��邽�߂̃R�[���o�b�N�֐�
/// </summary>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
/// <param name="msg">���b�Z�[�W</param>
/// <param name="wParam">�p�����[�^</param>
/// <param name="lParam">�p�����[�^</param>
/// <returns></returns>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// <summary>
/// �e�E�B���h�E�ɋ��ʂ̏������L�q�B�����WNDCLASS�ɓn��
/// </summary>
/// <param name="hWnd"></param>
/// <param name="msg"></param>
/// <param name="wParam"></param>
/// <param name="lParam">�C���X�^���X���擾���邽�߂ɁACreateWindow��lpParam��this��n���K�v������</param>
/// <returns></returns>
LRESULT WindowContext::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WindowContext* pThis;
	if (msg == WM_NCCREATE)
	{
		//�쐬���̃p�����[�^����this���擾�A�L���X�g
		pThis = static_cast<WindowContext*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		//this��USERDATA��this��R�t����
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->hWnd_ = hWnd;
	}
	else
	{
		//hWnd��this��R�Â��Ă������̂Ŏ擾
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
	case WM_DESTROY:  // �E�B���h�E�����
		Game::Exit();
		return S_OK;
	case WM_MOUSEMOVE:  // �}�E�X��������
		Game::System<Input>().UpdateMousePositionData(LOWORD(lParam), HIWORD(lParam));
		return S_OK;
	case WM_SIZE:  // �E�B���h�E�T�C�Y���ς����

		return S_OK;
	case WM_NCCALCSIZE:
		// wParam�ɂ��āFTRUE�Ȃ�NCCALCSIZE_PARAMS / FALSE�Ȃ�Rect*
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
			return HTCLIENT; // �T�C�Y�͕ς������Ȃ�
		default:
			return hitResult;
		}
	}
	default:  // ����ȊO�̃��b�Z�[�W�͏��n
		break;
	}


	// NOTE: ���ꂪ�����Ă���ƃE�B���h�E�\������Ȃ����A�G���[�͏o�Ȃ����ŋ�J����(����)
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


