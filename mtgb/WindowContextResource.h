#pragma once
#include <tuple>
#include <map>
#include <typeindex>
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
		virtual ~WindowContextResource() {};
		virtual void Initialize(WindowContext _windowContext) = 0;
		virtual void SetResource() = 0;
		virtual WindowContextResource* Clone() const= 0;
	};
}