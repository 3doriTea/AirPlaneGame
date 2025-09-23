#include "DXGIResource.h"
#include "ReleaseUtility.h"
#include "WindowContextUtil.h"
#include "Debug.h"
#include "MTImGui.h"
#include <processthreadsapi.h>
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
	Release();
}

//mtgb::DXGIResource::DXGIResource(const DXGIResource& other)
//	:WindowContextResource(other)
//	/*,pSwapChain1_{nullptr}
//	,pOutput_{nullptr}
//	,pDXGISurface_{nullptr}*/
//{
//}

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
	
	DWORD processId = GetCurrentProcessId();

	if (isMultiMonitor_ && processId > 0)
	{	
		
		std::optional<MonitorInfo> optMonitorInfo = dx11Manager.AssignAvailableMonitor(pOutput_.ReleaseAndGetAddressOf());
		if (optMonitorInfo)
		{
			monitorInfo_ = *optMonitorInfo;
		}

		HRESULT hResult = pOutput_->GetDesc(&outputDesc_);
		massert(SUCCEEDED(hResult)
			&& "GetDescに失敗 @DXGIResource::Initialize");

		// ボーダレスウィンドウにするならDescだけ取得して解放
		if (isBorderlessWindow)
		{
			pOutput_.Reset();
			
		}
		/*UINT nomModes = 0;
		pOutput_->GetDisplayModeList(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			0,
			&nomModes,
			nullptr);

		modeList_.resize(nomModes);

		pOutput_->GetDisplayModeList(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			0,
			&nomModes,
			modeList_.data());*/

		
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

void mtgb::DXGIResource::Update()
{
	MTImGui::Instance().DirectShow([this]
		{
			// モニター(DXGIOutput)の情報
			ImGui::PushID(&monitorInfo_);
			ImGui::LabelText("adapterIndex", "%d", monitorInfo_.adapterIndex);
			ImGui::LabelText("outputIndex", "%d", monitorInfo_.outputIndex);
			TypeRegistry::Instance().CallFunc(&monitorInfo_.desc, "OutputDesc");
			ImGui::PopID();
			ImGui::Separator();

			/*for (int i = 0; i < modeList_.size(); i++)
			{
				DXGI_MODE_DESC& modeDesc = modeList_[i];
				ImGui::Text("DXGI_MODE_DESC");
				ImGui::PushID(&modeDesc);
				ImGui::Text("Width,Height:%u,%u",modeDesc.Width,modeDesc.Height);
				ImGui::Text("RefreshRate:%u/%u",modeDesc.RefreshRate.Numerator,modeDesc.RefreshRate.Denominator);
				ImGui::PopID();
			}*/
		}, name_.c_str(), ShowType::Settings);
}

void mtgb::DXGIResource::Reset()
{
	pDXGISurface_.Reset();
	//pOutput_.Reset();
}

void mtgb::DXGIResource::OnResize(WindowContext _windowContext, UINT _width, UINT _height)
{
	HRESULT hResult = pSwapChain1_->ResizeBuffers(
		0, _width, _height, DXGI_FORMAT_UNKNOWN, 0);
	massert(SUCCEEDED(hResult) && "ResizeBuffersに失敗 @DXGIResource::OnResize");

	//再取得
	Game::System<DirectX11Manager>().CreateDXGISurface(pSwapChain1_.Get(), pDXGISurface_.ReleaseAndGetAddressOf());
}

void mtgb::DXGIResource::Release()
{
	// REF:https://learn.microsoft.com/ja-jp/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#full-screen-performance-tip
	// SwapChainはフルスクリーンモードでは解放できないらしい
	if (pSwapChain1_)
	{
		pSwapChain1_->SetFullscreenState(false, nullptr);
	}
	pSwapChain1_.Reset();
	pOutput_.Reset();
	pDXGISurface_.Reset();
}

//void mtgb::DXGIResource::SetFullscreen(bool _fullscreen)
//{
//	HRESULT hResult;
//	if (_fullscreen)
//	{
//		const RECT& rc = outputDesc_.DesktopCoordinates;
//		
//		DXGI_MODE_DESC modeDesc =
//		{
//			.Width = static_cast<UINT>(rc.right - rc.left),
//			.Height = static_cast<UINT>(rc.bottom - rc.top),
//			.RefreshRate
//			{
//				.Numerator = 60,
//				.Denominator = 1
//			},
//			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
//			// スキャンライン順序の設定
//			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, // 指定なし
//			// 画像の拡大方法の設定
//			// REF:https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/legacy/bb173066(v=vs.85)
//			// UNSPECIFIED以外だとフルスクリーンに切り替える際にモード変更(?)が発生する可能性があるらしい
//			.Scaling = DXGI_MODE_SCALING_UNSPECIFIED // 指定なし
//		};
//		
//		// 旧:最初のモードを使用
//		//hResult = pSwapChain1_->ResizeTarget(&modeList_[0]);
//		//hResult = pSwapChain1_->ResizeTarget(&modeDesc);
//		
//	}
//	
//	//hResult = pSwapChain1_->SetFullscreenState(_fullscreen, _fullscreen ? pOutput_.Get() : nullptr);
//	/*if (FAILED(hResult))
//	{
//		LOGIMGUI("WARN:%ld", hResult);
//	}*/
//	
//}

void mtgb::DXGIResource::SwapMonitorInfo(DXGIResource& _other)
{
	std::swap(monitorInfo_, _other.monitorInfo_);
	std::swap(outputDesc_, _other.outputDesc_);
}

WindowContextResource* mtgb::DXGIResource::Clone() const
{
	return new DXGIResource(*this);
}


