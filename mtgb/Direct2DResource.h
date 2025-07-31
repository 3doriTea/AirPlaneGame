#pragma once
#include "WindowContextResource.h"
#include "Direct3DResource.h"
#include "MTAssert.h"
#include "Direct2D/Direct2D.h"
#include "Game.h"
#include "ISystem.h"
#include <d2d1.h>
#include "WindowContext.h"

typedef struct HWND__* HWND;

namespace mtgb
{

	class Direct2DResource : public WindowContextResource
	{
	public:
		template<typename... Args>
		void Initialize(std::tuple<Args*...>& _resourceTuple, WindowContext _windowContext);
		void SetResource() override;
	private:
		ID2D1RenderTarget* pRenderTarget_;
		ID2D1SolidColorBrush* pD2DBrush_;
	};

	template<typename ...Args>
	inline void Direct2DResource::Initialize(std::tuple<Args*...>& _resourceTuple, WindowContext _windowContext)
	{
		// Direct3DResource‚©‚çˆË‘¶ƒŠƒ\[ƒX‚ğæ“¾
		Direct3DResource* d3dResource = std::get<Direct3DResource*>(_resourceTuple);
		if (!d3dResource) {
			massert(false && "Direct3DResource‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ @Direct2DResource::Initialize");
			return;
		}

		DXGIResource* dxgiResource = std::get<DXGIResource*>(_resourceTuple);
		if (!dxgiResource) {
			massert(false && "DXGIResource‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ @Direct2DResource::Initialize");
			return;
		}

		// Direct2D‚Ì‰Šú‰»ˆ—
		Direct2D& direct2D = Game::System<Direct2D>();

		direct2D.CreateD2DRenderTarget(dxgiResource->pDXGISurface_, &pRenderTarget_);
		direct2D.CreateSolidColorBrush(D2D1::ColorF::White, pRenderTarget_, &pD2DBrush_);
	}
}