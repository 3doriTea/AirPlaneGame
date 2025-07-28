#include "Direct2D.h"
#include "MTAssert.h"
#include "DirectX11Draw.h"
#include "ReleaseUtility.h"

#include <d3d11_1.h>

#include <d3d11.h>
#include <d2d1.h>
#include <Windows.h>
//#include <dwrite.h>  // direct write のやつ

#pragma comment(lib,"d3d11.lib")
//#pragma comment(lib,"d3dx11.lib")
//#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"d2d1.lib")

ID2D1Factory* mtgb::Direct2D::pD2DFactory_{ nullptr };
IDXGISurface* mtgb::Direct2D::pDXGISurface_{ nullptr };
ID2D1SolidColorBrush* mtgb::Direct2D::pD2DBrush_{ nullptr };
ID2D1RenderTarget* mtgb::Direct2D::pRenderTarget_{ nullptr };

void mtgb::Direct2D::Initialize()
{
	//ファクトリー作成
	HRESULT hResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory_);
	
	massert(SUCCEEDED(hResult)
		&& "D2D1CreateFactoryに失敗 @Direct2D::Initialize");

	//バックバッファ受け取る
	ID3D11Texture2D* pBackBuffer = nullptr;
	hResult = DirectX11Draw::pSwapChain1_->GetBuffer(0, _uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBuffer);

	massert(SUCCEEDED(hResult)
		&& "GetBufferに失敗 @Direct2D::Initialize");

	//バックバッファからIDXGISurfaceインターフェースを取り出す
	hResult = pBackBuffer->QueryInterface(IID_PPV_ARGS(&pDXGISurface_));

	SAFE_RELEASE(pBackBuffer);

	massert(SUCCEEDED(hResult)
		&& "QueryInterfaceに失敗 @Direct2D::Initialize");

	//Direct2Dのレンダーターゲット作成

	//レンダーターゲットのプロパティ設定書く
	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties(

		D2D1_RENDER_TARGET_TYPE_DEFAULT,//レンダリングモード。DEFAULTだと使用可能ならGPU、それ以外はCPUでレンダリング

		D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),//ピクセルの形式、アルファ値

		0,//x方向のdpi
		0,//y方向のdpi

		D2D1_RENDER_TARGET_USAGE_NONE,//何かしらの利用方法を設定

		D2D1_FEATURE_LEVEL_DEFAULT//最小の機能レベルを指定

	);

	//レンダーターゲット作成
	hResult = pD2DFactory_->CreateDxgiSurfaceRenderTarget(pDXGISurface_, renderTargetProperties, &pRenderTarget_);

	massert(SUCCEEDED(hResult)
		&& "CreateDxgiSurfaceRenderTargetに失敗 @Direct2D::Initialize");

	//ブラシ作成
	pRenderTarget_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White),
		&pD2DBrush_);
	//pRenderTarget_->SetTransform()
}

void mtgb::Direct2D::Update()
{

}

void mtgb::Direct2D::Draw()
{
}

void mtgb::Direct2D::Release()
{
	SAFE_RELEASE(pD2DFactory_);
	SAFE_RELEASE(pDXGISurface_);
	SAFE_RELEASE(pD2DBrush_);
	SAFE_RELEASE(pRenderTarget_);

}
