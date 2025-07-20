#pragma once
#include "ISystem.h"
#include "cmtgb.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib, "Xinput.lib")

typedef struct IDirectInput8A* LPDIRECTINPUT8;
typedef struct IDirectInputDevice8A* LPDIRECTINPUTDEVICE8;

namespace mtgb
{
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

	private:
		LPDIRECTINPUT8 pDirectInput_;        // Direct Input 本体k
		LPDIRECTINPUTDEVICE8 pKeyDevice_;    // キーデバイス
		LPDIRECTINPUTDEVICE8 pMouseDevice_;  // マウスデバイス
	};
}
