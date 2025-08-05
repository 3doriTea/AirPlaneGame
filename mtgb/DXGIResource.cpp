#include "DXGIResource.h"
#include "ReleaseUtility.h"
#include "WindowContextUtil.h"
using namespace mtgb;

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

	// マルチモニター対応は今回false固定
	bool isMultiMonitor = false;

	if (isMultiMonitor) {
		// 将来的にマルチモニター対応する場合のoutputIndexを管理
		int outputIndex = 0; // 仮の値
		IDXGIOutput* pRawDXGIOutput = nullptr;
		dx11Manager.CreateOutput(outputIndex, &pRawDXGIOutput);
		pOutput_.Attach(pRawDXGIOutput);
	}
	else {
		pOutput_ = nullptr;
	}

	IDXGISwapChain1* pRawSwapChain1 = nullptr;
	// スワップチェーンを作成
	dx11Manager.CreateSwapChain(hWnd, pOutput_.Get(), &pRawSwapChain1);
	pSwapChain1_.Attach(pRawSwapChain1);

	IDXGISurface* pRawSurface = nullptr;
	//サーフェスとやらを作成
	dx11Manager.CreateDXGISurface(pSwapChain1_.Get(), &pRawSurface);
	pDXGISurface_.Attach(pRawSurface);

}

void DXGIResource::SetResource()
{
	Game::System<DirectX11Manager>().ChangeSwapChain(pSwapChain1_);
}

WindowContextResource* mtgb::DXGIResource::Clone() const
{
	return new DXGIResource(*this);
}


