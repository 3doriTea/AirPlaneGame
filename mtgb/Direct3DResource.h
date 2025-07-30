#pragma once
#include "WindowContextResource.h"
#include "DXGIResource.h"
#include "MTAssert.h"

namespace mtgb
{
	class Direct3DResource : public WindowContextResource
	{
	public:
		template<typename... Args>
		void Initialize(std::tuple<Args*...>& _resourceTuple, HWND _hWnd);
		void SetResource() override;
	private:
		D3D11_VIEWPORT viewPort_;
		ID3D11RenderTargetView* pRenderTargetView_;
		ID3D11Texture2D* pDepthStencil_;
		ID3D11DepthStencilView* pDepthStencilView_;
	};


	template<typename ...Args>
	inline void Direct3DResource::Initialize(std::tuple<Args*...>& _resourceTuple, HWND _hWnd)
	{
		// DirectX11Managerにアクセス
		auto& dx11Manager = Game::System<DirectX11Manager>();

		// DXGIResourceから依存リソースを取得
		DXGIResource* dxgi = std::get<DXGIResource*>(_resourceTuple);
		if (!dxgi) {
			massert(false && "DXGIResourceが見つかりません @Direct3DResource::Initialize");
			return;
		}

		// レンダーターゲットビューを作成
		dx11Manager.CreateRenderTargetView(dxgi->pSwapChain1_, &pRenderTargetView_);

		// ビューポートを作成
		dx11Manager.CreateViewport(viewPort_);

		// 深度ステンシルと深度ステンシルビューを作成
		dx11Manager.CreateDepthStencilAndDepthStencilView(&pDepthStencil_, &pDepthStencilView_);
	}

	

}