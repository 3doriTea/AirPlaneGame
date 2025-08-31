#include "DXGIResource.h"
#include "ReleaseUtility.h"
#include "WindowContextUtil.h"
#include "Debug.h"
#include "MTImGui.h"
using namespace mtgb;


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
	if (_windowContext == WindowContext::First)
	{
		name_ = "FirstWindowDXGI";
	}
	else if (_windowContext == WindowContext::Second)
	{
		name_ = "SecondWindowDXGI";
	}

	// DirectX11Managerにアクセスしてリソースを作成
	auto& dx11Manager = Game::System<DirectX11Manager>();

	HWND hWnd = WinCtxRes::GetHWND(_windowContext);

	// マルチモニター対応するかどうか
	bool isMultiMonitor = true;

	if (isMultiMonitor) {
		// 将来的にマルチモニター対応する場合のoutputIndexを管理
		
		outputMonitorIndex_ = dx11Manager.AssignAvailableMonitor(pOutput_.ReleaseAndGetAddressOf());

		HRESULT hResult = pOutput_->GetDesc(&outputDesc_);
		massert(SUCCEEDED(hResult)
			&& "GetDescに失敗 @DXGIResource::Initialize");
	}
	else 
	{
		pOutput_ = nullptr;
		outputMonitorIndex_ = -1;
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

void mtgb::DXGIResource::Update()
{
	MTImGui::Instance().TypedShow(&outputDesc_, name_.c_str(), ShowType::Inspector);
	//MTImGui::Instance()
}

void mtgb::DXGIResource::SetFullscreen(bool _fullscreen)
{
	HRESULT hResult;
	if (_fullscreen)
	{
		hResult = pSwapChain1_->SetFullscreenState(_fullscreen, pOutput_.Get());
	}
	else
	{
		hResult = pSwapChain1_->SetFullscreenState(_fullscreen, nullptr);
	}
	
	if (FAILED(hResult))
	{
		LOGIMGUI("WARN:%ld", hResult);
	}
}

WindowContextResource* mtgb::DXGIResource::Clone() const
{
	return new DXGIResource(*this);
}


