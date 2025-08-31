#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include <dxgi1_2.h>
#include "WindowContext.h"
#include <map>
#include <typeindex>
#include <string>
typedef struct HWND__* HWND;

namespace mtgb
{

	class DXGIResource : public WindowContextResource
	{
	public:
		DXGIResource();
		~DXGIResource();
		DXGIResource(const DXGIResource& other);
		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;
		void Update() override;
		/// <summary>
		/// フルスクリーンの切り替え
		/// </summary>
		/// <param name="_fullscreen">trueならフルスクリーンになり、falseならウィンドウモード</param>
		void SetFullscreen(bool _fullscreen);

		ComPtr<IDXGISwapChain1> pSwapChain1_;
		ComPtr<IDXGIOutput> pOutput_;
		ComPtr<IDXGISurface> pDXGISurface_;

		// WindowContextResource を介して継承されました
		
		unsigned int outputMonitorIndex_;
		// WindowContextResource を介して継承されました
		WindowContextResource* Clone() const override;
	private:
		std::string name_;
		DXGI_OUTPUT_DESC outputDesc_;
	};
}