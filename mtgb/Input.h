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
		/// マウスの座標データを更新する
		/// </summary>
		/// <param name="_x">座標 x</param>
		/// <param name="_y">座標 y</param>
		void UpdateMousePositionData(const int32_t _x, const int32_t _y);

		void CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice);
		void CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice);
		void ChangeKeyDevice(LPDIRECTINPUTDEVICE8 _pKeyDevice);
		void ChangeMouseDevice(LPDIRECTINPUTDEVICE8 _pMouseDevice);
		void ChangeInputData(InputData* _pInputData);
	private:
		InputData* pInputData_;				 // 入力の状態
		LPDIRECTINPUT8 pDirectInput_;        // Direct Input 本体k
		LPDIRECTINPUTDEVICE8 pKeyDevice_;    // キーデバイス
		LPDIRECTINPUTDEVICE8 pMouseDevice_;  // マウスデバイス
	};
}
