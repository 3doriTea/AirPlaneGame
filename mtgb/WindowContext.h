#pragma once
#include "string"
#include <d3d11.h>
#include <Windows.h>
#include <dxgi.h>
#include <Dxgi1_2.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace mtgb
{
	enum class WindowContext
	{
		First,
		Second,
		Both,
		None
	};
}
