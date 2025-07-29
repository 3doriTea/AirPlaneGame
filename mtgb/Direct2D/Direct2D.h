#pragma once
#include "cmtgb.h"
#include <d2d1.h>


struct ID2D1Factory;
struct IDXGISurface;
struct ID2D1SolidColorBrush;
struct ID2D1RenderTarget;
struct IDXGISwapChain1;


namespace mtgb
{
	
	class Direct2D : public ISystem
	{
		friend class DirectWrite;
	public:
		void Initialize() override;
		void InitializeCommonResource();
		void CreateD2DRenderTarget(IDXGISurface* pIDXGISurface, ID2D1RenderTarget** ppRenderTarget_);
		void CreateSolidColorBrush(const D2D1::ColorF& color, ID2D1RenderTarget* pRenderTarget, ID2D1SolidColorBrush** ppD2DBrush);

		void ChangeRenderTarget(ID2D1SolidColorBrush* pD2DBrush, ID2D1RenderTarget* pRenderTarget);

		void Update() override;
		void Draw();
		void Release();

	private:
		static ID2D1Factory* pD2DFactory_;
		static IDXGISurface* pDXGISurface_;
		static ID2D1SolidColorBrush* pD2DBrush_;
		static ID2D1RenderTarget* pRenderTarget_;
	};
}
