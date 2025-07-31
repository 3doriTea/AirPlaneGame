#pragma once
#include "WindowContextResource.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include <dxgi1_2.h>
#include "WindowContext.h"
typedef struct HWND__* HWND;

namespace mtgb
{
	class DXGIResource : public WindowContextResource
	{
	public:
		template<typename... Args>
		void Initialize(std::tuple<Args*...>& _resourceTuple, WindowContext _windowContext);
		void SetResource() override;

		IDXGISwapChain1* pSwapChain1_;
		IDXGIOutput* pOutput_;
		IDXGISurface* pDXGISurface_;
	};
	
	template<typename ...Args>
	inline void DXGIResource::Initialize(std::tuple<Args*...>& _resourceTuple, WindowContext _windowContext)
	{
		// DirectX11Managerにアクセスしてリソースを作成
		auto& dx11Manager = Game::System<DirectX11Manager>();

		// WindowContextResourceManagerからHWNDを取得
		HWND hWnd = Game::System<WindowContextResourceManager>().GetHWND(_windowContext);

		// 依存関係は特にない（DXGIResourceは最初に初期化される）
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
}