#pragma once
#include "ISystem.h"
#include "cmtgb.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib, "Xinput.lib")

typedef struct IDirectInput8A* LPDIRECTINPUT8;
typedef struct IDirectInputDevice8A* LPDIRECTINPUTDEVICE8;
typedef struct HWND__* HWND;

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
		void ChangeKeyDevice(LPDIRECTINPUTDEVICE8 _pKeyDevice);
		void ChangeMouseDevice(LPDIRECTINPUTDEVICE8 _pMouseDevice);
		void ChangeInputData(InputData* _pInputData);
	private:
		InputData* pInputData_;				 // ���͂̏��
		LPDIRECTINPUT8 pDirectInput_;        // Direct Input �{��k
		LPDIRECTINPUTDEVICE8 pKeyDevice_;    // �L�[�f�o�C�X
		LPDIRECTINPUTDEVICE8 pMouseDevice_;  // �}�E�X�f�o�C�X
	};
}
