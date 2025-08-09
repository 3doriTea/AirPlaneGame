#pragma once
#include "IncludingInput.h"
#include "ImGuiShowable.h"
#include <string>
namespace mtgb
{
	/// <summary>
	/// ジョイスティックの入力をImGuiで表示する用
	/// </summary>
	struct JoystickProxy : public ImGuiShowable<JoystickProxy>
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

		std::string connectionStatus;
		std::string assignmentStatus;
		std::string lastErrorMessage;
		bool isConnected;
		bool isAssigned;

		void UpdateFromInput(GUID guid);
		void UpdateInputData(const DIJOYSTATE& js);
		//JoystickProxy operator=(const DIJOYSTATE& js);
	};
}