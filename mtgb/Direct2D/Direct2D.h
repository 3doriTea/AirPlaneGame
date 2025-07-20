#pragma once
#include <d3d11_1.h>
#include <d2d1.h>
#include <Windows.h>
#include <dwrite.h>  // direct write ‚Ì‚â‚Â

namespace mtgb
{
	class Direct2D
	{
	private:
		IDXGIDevice* pDXGIDevice_;
		ID2D1Factory* pD2DFactory_;
		ID2D1Device* pD2DDevice_;

	};
}
