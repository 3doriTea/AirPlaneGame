#pragma once
#include <tuple>
#include <d3d11.h>

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
		void Initialize(std::tuple<Args*...>& _resourceTuple);
		virtual void SetResource() = 0;
	};
}







