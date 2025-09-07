#include "Direct3DResource.h"
#include "ReleaseUtility.h"
#include "Screen.h"
#include "WindowManager.h"
using namespace mtgb;

mtgb::Direct3DResource::Direct3DResource()
	:pRenderTargetView_{nullptr},pDepthStencil_{nullptr},pDepthStencilView_{nullptr}
{

}

mtgb::Direct3DResource::~Direct3DResource()
{
	Release();
}

//mtgb::Direct3DResource::Direct3DResource(const Direct3DResource& other)
//	:WindowContextResource(other)
//	,pRenderTargetView_{nullptr}
//	,pDepthStencil_{nullptr}
//	,pDepthStencilView_{nullptr}
//	,viewPort_{other.viewPort_ }
//{
//
//}

void mtgb::Direct3DResource::Initialize(WindowContext _windowContext)
{
	// DirectX11Managerにアクセス
	auto& dx11Manager = Game::System<DirectX11Manager>();

	DXGIResource& dxgi = Game::System<WindowContextResourceManager>().Get<DXGIResource>(_windowContext);

	// レンダーターゲットビューを作成
	dx11Manager.CreateRenderTargetView(dxgi.pSwapChain1_.Get(), pRenderTargetView_.ReleaseAndGetAddressOf());

	// ビューポートを作成
	const Vector2Int SCREEN_SIZE{ Game::System<WindowManager>().GetWindowSize(_windowContext)};
	dx11Manager.CreateViewport(SCREEN_SIZE, viewPort_);

	// 深度ステンシルと深度ステンシルビューを作成
	dx11Manager.CreateDepthStencilAndDepthStencilView(SCREEN_SIZE, pDepthStencil_.ReleaseAndGetAddressOf(), pDepthStencilView_.ReleaseAndGetAddressOf());	
}

void mtgb::Direct3DResource::SetResource()
{
	DirectX11Manager& dx11Manager{ Game::System<DirectX11Manager>() };
	dx11Manager.ChangeViewport(viewPort_);
	dx11Manager.ChangeRenderTargets(pRenderTargetView_, pDepthStencilView_);
}

void mtgb::Direct3DResource::Reset()
{
	// 既存リリースを解放
	Release();
}

void mtgb::Direct3DResource::OnResize(WindowContext _windowContext, UINT _width, UINT _height)
{
	// DirectX11Managerにアクセス
	auto& dx11Manager = Game::System<DirectX11Manager>();

	DXGIResource& dxgi = Game::System<WindowContextResourceManager>().Get<DXGIResource>(_windowContext);

	// レンダーターゲットビューを作成
	dx11Manager.CreateRenderTargetView(dxgi.pSwapChain1_.Get(), pRenderTargetView_.ReleaseAndGetAddressOf());

	// ビューポートを作成
	const Vector2Int SCREEN_SIZE{ static_cast<int>(_width),static_cast<int>(_height) };
	dx11Manager.CreateViewport(SCREEN_SIZE, viewPort_);

	// 深度ステンシルと深度ステンシルビューを作成
	dx11Manager.CreateDepthStencilAndDepthStencilView(SCREEN_SIZE, pDepthStencil_.ReleaseAndGetAddressOf(), pDepthStencilView_.ReleaseAndGetAddressOf());
}

void mtgb::Direct3DResource::Release()
{
	pDepthStencil_.Reset();
	pDepthStencilView_.Reset();
	pRenderTargetView_.Reset();
}



const D3D11_VIEWPORT& mtgb::Direct3DResource::GetViewport()
{
	return viewPort_;
}

Direct3DResource* mtgb::Direct3DResource::Clone() const
{
	return new Direct3DResource(*this);
}


