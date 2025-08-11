#include "Direct3DResource.h"
#include "ReleaseUtility.h"
#include "Screen.h"
using namespace mtgb;

mtgb::Direct3DResource::Direct3DResource()
	:pRenderTargetView_{nullptr},pDepthStencil_{nullptr},pDepthStencilView_{nullptr}
{

}

mtgb::Direct3DResource::~Direct3DResource()
{
	pRenderTargetView_.Reset();
	pDepthStencilView_.Reset();
	pDepthStencil_.Reset();
}

mtgb::Direct3DResource::Direct3DResource(const Direct3DResource& other)
	:WindowContextResource(other)
	,pRenderTargetView_{nullptr}
	,pDepthStencil_{nullptr}
	,pDepthStencilView_{nullptr}
	,viewPort_{other.viewPort_ }
{

}

void mtgb::Direct3DResource::Initialize(WindowContext _windowContext)
{
	// DirectX11Managerにアクセス
	auto& dx11Manager = Game::System<DirectX11Manager>();

	DXGIResource& dxgi = Game::System<WindowContextResourceManager>().Get<DXGIResource>(_windowContext);

	// レンダーターゲットビューを作成
	ID3D11RenderTargetView* pRawRenderTargetView = nullptr;
	dx11Manager.CreateRenderTargetView(dxgi.pSwapChain1_.Get(), &pRawRenderTargetView);
	pRenderTargetView_.Attach(pRawRenderTargetView);

	// ビューポートを作成
	dx11Manager.CreateViewport(viewPort_);

	// 深度ステンシルと深度ステンシルビューを作成
	ID3D11Texture2D* pRawDepthStencil = nullptr;
	ID3D11DepthStencilView* pRawDepthStencilView = nullptr;

	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	dx11Manager.CreateDepthStencilAndDepthStencilView(SCREEN_SIZE, &pRawDepthStencil, &pRawDepthStencilView);
	pDepthStencil_.Attach(pRawDepthStencil);
	pDepthStencilView_.Attach(pRawDepthStencilView);
	
}

void mtgb::Direct3DResource::SetResource()
{
	DirectX11Manager& dx11Manager{ Game::System<DirectX11Manager>() };
	dx11Manager.ChangeViewport(viewPort_);
	dx11Manager.ChangeRenderTargets(pRenderTargetView_, pDepthStencilView_);
}

Direct3DResource* mtgb::Direct3DResource::Clone() const
{
	return new Direct3DResource(*this);
}


