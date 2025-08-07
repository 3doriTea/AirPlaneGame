#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <dinput.h>
#include <wrl/client.h> // 追加
#include <Xinput.h>


#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib, "xinput.lib")

typedef struct HWND__* HWND;
using Microsoft::WRL::ComPtr; // 追加

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
		/// マウスの座標データを更新する
		/// </summary>
		/// <param name="_x">座標 x</param>
		/// <param name="_y">座標 y</param>
		void UpdateMousePositionData(const int32_t _x, const int32_t _y);

		void CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice);
		void CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice);
		void ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice);
		void ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice);
		void ChangeInputData(InputData* _pInputData);
	private:
		InputData*                  pInputData_;    // 入力の状態
		ComPtr<IDirectInput8>       pDirectInput_;  // Direct Input 本体
		ComPtr<IDirectInputDevice8> pKeyDevice_;    // キーデバイス
		ComPtr<IDirectInputDevice8> pMouseDevice_;  // マウスデバイス
		PXINPUT_STATE               pXInputData_;   // XInputデバイスの入力状態
	};
}
