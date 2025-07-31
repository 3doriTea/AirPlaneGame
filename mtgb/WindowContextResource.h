#pragma once
#include <tuple>
#include <d3d11.h>
#include "WindowContext.h"

struct IDXGISwapChain1;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID2D1RenderTarget;

namespace mtgb
{
	class WindowContextResource
	{
	public:
		template<typename... Args>
		void Initialize(std::tuple<Args*...>& _resourceTuple, WindowContext _windowContext);
		virtual void SetResource() = 0;
	};
}







