#pragma once
#include "IncludingInput.h"
#include "ImGuiShowable.h"
#include <string>
namespace mtgb
{
	/// <summary>
	/// ジョイスティックの入力をImGuiで表示する用
	/// </summary>
	struct JoystickProxy 
	{
		JoystickProxy(const DIJOYSTATE& js);
		LONG lX;
		LONG lY;
		LONG lZ;
		LONG lRx;// 左のトリガーボタン
		LONG lRy;// 右のトリガーボタン
		LONG lRz;
		LONG rglSlider[2];
		DWORD rgdwPOV[4];
		BYTE rgbButtons[32];

		std::string connectionStatus;
		std::string assignmentStatus;
		std::string lastErrorMessage;
		std::string deviceName;
		std::string deviceProductName;
		bool isConnected;
		bool isAssigned;

		void UpdateFromInput(GUID guid);
		void UpdateInputData(const DIJOYSTATE& js);
		//JoystickProxy operator=(const DIJOYSTATE& js);
	};

	// Dual Shockの場合
	// lRx,lRyが左、右のトリガーボタン
	// lYは上に倒すと負、下で正、lXは左が負、右が正
}