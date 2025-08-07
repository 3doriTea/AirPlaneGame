#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <dinput.h>
#include <wrl/client.h> // �ǉ�
#include <Xinput.h>


#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib, "xinput.lib")

typedef struct HWND__* HWND;
using Microsoft::WRL::ComPtr; // �ǉ�

namespace mtgb
{
	class InputResource;
	class InputData;
	class Input : public ISystem
	{
	public:
		Input();
		~Input();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// �}�E�X�̍��W�f�[�^���X�V����
		/// </summary>
		/// <param name="_x">���W x</param>
		/// <param name="_y">���W y</param>
		void UpdateMousePositionData(const int32_t _x, const int32_t _y);

		void CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice);
		void CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice);
		void ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice);
		void ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice);
		void ChangeInputData(InputData* _pInputData);
	private:
		InputData*                  pInputData_;    // ���͂̏��
		ComPtr<IDirectInput8>       pDirectInput_;  // Direct Input �{��
		ComPtr<IDirectInputDevice8> pKeyDevice_;    // �L�[�f�o�C�X
		ComPtr<IDirectInputDevice8> pMouseDevice_;  // �}�E�X�f�o�C�X
		PXINPUT_STATE               pXInputData_;   // XInput�f�o�C�X�̓��͏��
	};
}
