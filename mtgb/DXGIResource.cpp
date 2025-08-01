#include "DXGIResource.h"

using namespace mtgb;

mtgb::DXGIResource::DXGIResource()
	:pSwapChain1_{nullptr}, pOutput_{nullptr}, pDXGISurface_{nullptr}
{
}

mtgb::DXGIResource::DXGIResource(const DXGIResource& other)
	:WindowContextResource(other)
	,pSwapChain1_{nullptr}
	,pOutput_{nullptr}
	,pDXGISurface_{nullptr}
{
}

void DXGIResource::Initialize(WindowContext _windowContext)
{
	// DirectX11Managerにアクセスしてリソースを作成
	auto& dx11Manager = Game::System<DirectX11Manager>();

	HWND hWnd = Game::System<WindowContextResourceManager>().GetHWND(_windowContext);

	// マルチモニター対応は今回false固定
	bool isMultiMonitor = false;

	if (isMultiMonitor) {
		// 将来的にマルチモニター対応する場合のoutputIndexを管理
		int outputIndex = 0; // 仮の値
		dx11Manager.CreateOutput(outputIndex, &pOutput_);
	}
	else {
		pOutput_ = nullptr;
	}

	// スワップチェーンを作成
	dx11Manager.CreateSwapChain(hWnd, pOutput_, &pSwapChain1_);

	//サーフェスとやらを作成
	dx11Manager.CreateDXGISurface(pSwapChain1_, &pDXGISurface_);
}

void DXGIResource::SetResource()
{
	Game::System<DirectX11Manager>().ChangeSwapChain(pSwapChain1_);
}

WindowContextResource* mtgb::DXGIResource::Clone() const
{
	return new DXGIResource(*this);
}


