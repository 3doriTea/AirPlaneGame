#include "Direct2D.h"
#include "MTAssert.h"
#include "DirectX11Draw.h"
#include "ReleaseUtility.h"

#include <d3d11_1.h>

#include <d3d11.h>
#include <d2d1.h>
#include <Windows.h>
//#include <dwrite.h>  // direct write �̂��

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
	//�t�@�N�g���[�쐬
	HRESULT hResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory_);
	
	massert(SUCCEEDED(hResult)
		&& "D2D1CreateFactory�Ɏ��s @Direct2D::Initialize");

	//�o�b�N�o�b�t�@�󂯎��
	ID3D11Texture2D* pBackBuffer = nullptr;
	hResult = DirectX11Draw::pSwapChain1_->GetBuffer(0, _uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBuffer);

	massert(SUCCEEDED(hResult)
		&& "GetBuffer�Ɏ��s @Direct2D::Initialize");

	//�o�b�N�o�b�t�@����IDXGISurface�C���^�[�t�F�[�X�����o��
	hResult = pBackBuffer->QueryInterface(IID_PPV_ARGS(&pDXGISurface_));

	SAFE_RELEASE(pBackBuffer);

	massert(SUCCEEDED(hResult)
		&& "QueryInterface�Ɏ��s @Direct2D::Initialize");

	//Direct2D�̃����_�[�^�[�Q�b�g�쐬

	//�����_�[�^�[�Q�b�g�̃v���p�e�B�ݒ菑��
	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties(

		D2D1_RENDER_TARGET_TYPE_DEFAULT,//�����_�����O���[�h�BDEFAULT���Ǝg�p�\�Ȃ�GPU�A����ȊO��CPU�Ń����_�����O

		D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),//�s�N�Z���̌`���A�A���t�@�l

		0,//x������dpi
		0,//y������dpi

		D2D1_RENDER_TARGET_USAGE_NONE,//��������̗��p���@��ݒ�

		D2D1_FEATURE_LEVEL_DEFAULT//�ŏ��̋@�\���x�����w��

	);

	//�����_�[�^�[�Q�b�g�쐬
	hResult = pD2DFactory_->CreateDxgiSurfaceRenderTarget(pDXGISurface_, renderTargetProperties, &pRenderTarget_);

	massert(SUCCEEDED(hResult)
		&& "CreateDxgiSurfaceRenderTarget�Ɏ��s @Direct2D::Initialize");

	//�u���V�쐬
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
