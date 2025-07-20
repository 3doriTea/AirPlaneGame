#pragma once
#include "cmtgb.h"



struct IDXGIDevice;
struct ID2D1Factory;
struct IDXGISurface;
struct ID2D1SolidColorBrush;
struct ID2D1RenderTarget;

namespace mtgb
{
	class Direct2D : public ISystem
	{
		friend class DirectWrite;
	public:
		void Initialize() override;
		void Update() override;
		void Draw();
		void Release();

	private:
		static IDXGIDevice* pDXGIDevice_;
		static ID2D1Factory* pD2DFactory_;
		static IDXGISurface* pDXGISurface_;
		static ID2D1SolidColorBrush* pD2DBrush_;
		static ID2D1RenderTarget* pRenderTarget_;

	};
}
