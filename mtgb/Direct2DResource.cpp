#include "Direct2DResource.h"
#include "ReleaseUtility.h"
using namespace mtgb;

mtgb::Direct2DResource::Direct2DResource()
	:pRenderTarget_{nullptr},pD2DBrush_{nullptr}
{

}

mtgb::Direct2DResource::~Direct2DResource()
{
	pD2DBrush_.Reset();
	pRenderTarget_.Reset();
}

mtgb::Direct2DResource::Direct2DResource(const Direct2DResource& other)
	:WindowContextResource{ other }
	,pRenderTarget_{nullptr}
	,pD2DBrush_{nullptr}
{
}

void mtgb::Direct2DResource::Initialize(WindowContext _windowContext)
{
	Direct3DResource& d3dResource = Game::System<WindowContextResourceManager>().Get<Direct3DResource>(_windowContext);
	
	DXGIResource& dxgiResource = Game::System<WindowContextResourceManager>().Get<DXGIResource>(_windowContext);

	Direct2D& direct2D = Game::System<Direct2D>();

	ID2D1RenderTarget* pRawRenderTarget = nullptr;
	direct2D.CreateD2DRenderTarget(dxgiResource.pDXGISurface_.Get(), &pRawRenderTarget);
	pRenderTarget_.Attach(pRawRenderTarget);

	ID2D1SolidColorBrush* pRawD2DBrush = nullptr;
	direct2D.CreateSolidColorBrush(D2D1::ColorF::White, pRenderTarget_.Get(), &pRawD2DBrush);
	pD2DBrush_.Attach(pRawD2DBrush);
}

void mtgb::Direct2DResource::SetResource()
{
	Game::System<Direct2D>().ChangeRenderTarget(pD2DBrush_, pRenderTarget_);
}

WindowContextResource* mtgb::Direct2DResource::Clone() const
{
	return new Direct2DResource(*this);
}

