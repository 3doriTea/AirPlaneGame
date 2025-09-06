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
#include "MonitorInfo.h"
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
		void Reset() override;
		void OnResize(WindowContext _windowContext, UINT _width, UINT _height) override;
		void Release() override;

		/// <summary>
		/// フルスクリーンの切り替え
		/// </summary>
		/// <param name="_fullscreen">trueならフルスクリーンになり、falseならウィンドウモード</param>
		void SetFullscreen(bool _fullscreen);

		/// <summary>
		/// 割り当てられたモニターの矩形を返す
		/// </summary>
		RECT GetAssignedMonitorRect() const { return outputDesc_.DesktopCoordinates; }

		ComPtr<IDXGISwapChain1> pSwapChain1_;
		ComPtr<IDXGIOutput> pOutput_;
		ComPtr<IDXGISurface> pDXGISurface_;

		MonitorInfo monitorInfo_;

		WindowContextResource* Clone() const override;
	private:
		bool isMultiMonitor_ = true;
		bool isBorderlessWindow = true;
		std::vector<DXGI_MODE_DESC> modeList_;
		std::string name_;
		DXGI_OUTPUT_DESC outputDesc_;
	};
}