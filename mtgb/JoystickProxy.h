#pragma once
#include "IncludingInput.h"
#include "ImGuiShowable.h"
namespace mtgb
{
	/// <summary>
	/// ジョイスティックの入力をImGuiで表示する用
	/// </summary>
	struct JoystickProxy : ImGuiShowable<JoystickProxy>
	{
		JoystickProxy(const DIJOYSTATE& js);
		LONG lX;
		LONG lY;
		LONG lZ;
		LONG lRx;
		LONG lRy;
		LONG lRz;
		LONG rglSlider[2];
		DWORD rgdwPOV[4];
		BYTE rgbButtons[32];

		JoystickProxy operator=(const DIJOYSTATE& js);
	};
}