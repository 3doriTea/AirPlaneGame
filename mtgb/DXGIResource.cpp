#include "DXGIResource.h"
#include "ReleaseUtility.h"
#include "WindowContextUtil.h"
#include "Debug.h"

using namespace mtgb;

// カウントは0からスタート
unsigned int DXGIResource::outputMonitorCounter_{ 0 };

namespace
{

}

mtgb::DXGIResource::DXGIResource()
	:pSwapChain1_{nullptr}, pOutput_{nullptr}, pDXGISurface_{nullptr}
{
}

mtgb::DXGIResource::~DXGIResource()
{
	pSwapChain1_.Reset();
	pOutput_.Reset();
	pDXGISurface_.Reset();
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

	HWND hWnd = WinCtxRes::GetHWND(_windowContext);

	// マルチモニター対応するかどうか
	bool isMultiMonitor = false;

	if (isMultiMonitor) {
		// 将来的にマルチモニター対応する場合のoutputIndexを管理
		outputMonitorIndex_ = outputMonitorCounter_++; // 仮の値
		dx11Manager.CreateOutput(outputMonitorIndex_, pOutput_.ReleaseAndGetAddressOf());
	}
	else 
	{
		pOutput_ = nullptr;
	}

	// スワップチェーンを作成
	dx11Manager.CreateSwapChain(hWnd, pOutput_.Get(), pSwapChain1_.ReleaseAndGetAddressOf());

	//サーフェスとやらを作成
	dx11Manager.CreateDXGISurface(pSwapChain1_.Get(), pDXGISurface_.ReleaseAndGetAddressOf());
}

void DXGIResource::SetResource()
{
	Game::System<DirectX11Manager>().ChangeSwapChain(pSwapChain1_);
}

void mtgb::DXGIResource::SetFullscreen(bool _fullscreen)
{
	HRESULT hResult = pSwapChain1_->SetFullscreenState(_fullscreen, pOutput_.Get());
	if (FAILED(hResult))
	{
		LOGIMGUI("WARN:%ld", hResult);
	}
}

WindowContextResource* mtgb::DXGIResource::Clone() const
{
	return new DXGIResource(*this);
}


