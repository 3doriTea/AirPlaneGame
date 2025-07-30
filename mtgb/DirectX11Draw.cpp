#include "DirectX11Draw.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include "Texture2D.h"
#include "ReleaseUtility.h"
#include "IShader.h"
#include "DirectWrite.h"
#include "OBJ.h"


ID3D11Device* mtgb::DirectX11Draw::pDevice_{ nullptr };                                                    // �`����s�����߂̊��A���\�[�X�̍쐬�Ɏg��
ID3D11DeviceContext* mtgb::DirectX11Draw::pContext_{ nullptr };   
IDXGIDevice1* mtgb::DirectX11Draw::pDXGIDevice_{ nullptr };
IDXGIAdapter* mtgb::DirectX11Draw::pDXGIAdapter_{ nullptr };
IDXGIFactory2* mtgb::DirectX11Draw::pDXGIFactory_{ nullptr };

IDXGISwapChain* mtgb::DirectX11Draw::pSwapChain_{ nullptr };                                               // �_�u���o�b�t�@�����O������
ID3D11RenderTargetView* mtgb::DirectX11Draw::pRenderTargetView_{ nullptr };                                // �`���
ID3D11DepthStencilView* mtgb::DirectX11Draw::pDepthStencilView_{ nullptr };                                // �[�x�o�b�t�@
ID3D11DepthStencilState* mtgb::DirectX11Draw::pDepthStencilState_[static_cast<int8_t>(BlendMode::Max)]{};  // �u�����h�ɂ��[�x�o�b�t�@�ւ̏������ݏ��
ID3D11Texture2D* mtgb::DirectX11Draw::pDepthStencil_{ nullptr };                                           // �u�����h�̏��
ID3D11BlendState* mtgb::DirectX11Draw::pBlendState_[static_cast<int8_t>(BlendMode::Max)]{};  // �u�����h�̏��
ID3D11SamplerState* mtgb::DirectX11Draw::pDefaultSamplerState_{ nullptr };
mtgb::ShaderBundle mtgb::DirectX11Draw::shaderBundle_[static_cast<int8_t>(ShaderType::Max)]{};             // �V�F�[�_�̃o���h��
mtgb::Vector4 mtgb::DirectX11Draw::backgroundColor_{ 0, 0, 0, 1 };

void mtgb::DirectX11Draw::SetShader(const ShaderType _type)
{
	const int INDEX{ static_cast<int>(_type) };
	pContext_->RSSetState(shaderBundle_[INDEX].pRasterizerState);
	pContext_->VSSetShader(shaderBundle_[INDEX].pVertexShader, nullptr, 0);
	pContext_->PSSetShader(shaderBundle_[INDEX].pPixelShader, nullptr, 0);
	pContext_->IASetInputLayout(shaderBundle_[INDEX].pVertexLayout);
}

void mtgb::DirectX11Draw::SetBlendMode(const BlendMode _mode)
{
	const int INDEX{ static_cast<int>(_mode) };

	// ���Z����
	float blendFactor[]{ D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	pContext_->OMSetBlendState(pBlendState_[INDEX], blendFactor, 0xffffffffU);

	// �[�x�X�e���V���ւ̏�������
	pContext_->OMSetDepthStencilState(pDepthStencilState_[INDEX], 0);
}

void mtgb::DirectX11Draw::SetIsWriteToDepthBuffer(const bool _enabled)
{
	if (_enabled)
	{
		// �[�x�o�b�t�@���w�肷��
		pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);
	}
	else
	{
		// �[�x�o�b�t�@���O�� nullptr���w�肷��
		pContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);
	}
}

void mtgb::DirectX11Draw::Begin()
{

	pContext_->ClearRenderTargetView(pRenderTargetView_, backgroundColor_.f);

	pContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH, 1, 0U);

}

void mtgb::DirectX11Draw::End()
{
	pSwapChain_->Present(0U, 0U);
}

void mtgb::DirectX11Draw::Release()
{
	SAFE_RELEASE(pDepthStencilView_);
	SAFE_RELEASE(pRenderTargetView_);
	SAFE_RELEASE(pDXGIDevice_);
	SAFE_RELEASE(pDXGIAdapter_);
	SAFE_RELEASE(pDXGIFactory_);
	SAFE_RELEASE(pSwapChain_);
	SAFE_RELEASE(pContext_);
	SAFE_RELEASE(pDevice_);
}
