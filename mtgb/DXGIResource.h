#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include <dxgi1_2.h>
#include "WindowContext.h"
#include <map>
#include <typeindex>

typedef struct HWND__* HWND;

namespace mtgb
{

	class DXGIResource : public WindowContextResource
	{
	public:
		DXGIResource();
		~DXGIResource();
		DXGIResource(const DXGIResource& other);
		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;

		ComPtr<IDXGISwapChain1> pSwapChain1_;
		ComPtr<IDXGIOutput> pOutput_;
		ComPtr<IDXGISurface> pDXGISurface_;

		// WindowContextResource ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		

		// WindowContextResource ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		WindowContextResource* Clone() const override;

	};
}