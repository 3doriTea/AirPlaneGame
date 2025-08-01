#include "Direct3DResource.h"

using namespace mtgb;

mtgb::Direct3DResource::Direct3DResource()
	:pRenderTargetView_{nullptr},pDepthStencil_{nullptr},pDepthStencilView_{nullptr}
{

}

mtgb::Direct3DResource::Direct3DResource(const Direct3DResource& other)
	:WindowContextResource(other)
	,pRenderTargetView_{nullptr}
	,pDepthStencil_{nullptr}
	,pDepthStencilView_{nullptr}
	,viewPort_{other.viewPort_ }
{

}

void mtgb::Direct3DResource::Initialize(WindowContext _windowContext)
{
	// DirectX11Manager�ɃA�N�Z�X
	auto& dx11Manager = Game::System<DirectX11Manager>();

	DXGIResource& dxgi = Game::System<WindowContextResourceManager>().Get<DXGIResource>(_windowContext);

	// �����_�[�^�[�Q�b�g�r���[���쐬
	dx11Manager.CreateRenderTargetView(dxgi.pSwapChain1_, &pRenderTargetView_);

	// �r���[�|�[�g���쐬
	dx11Manager.CreateViewport(viewPort_);

	// �[�x�X�e���V���Ɛ[�x�X�e���V���r���[���쐬
	dx11Manager.CreateDepthStencilAndDepthStencilView(&pDepthStencil_, &pDepthStencilView_);
}

void mtgb::Direct3DResource::SetResource()
{
	DirectX11Manager& dx11Manager{ Game::System<DirectX11Manager>() };
	dx11Manager.ChangeViewport(viewPort_);
	dx11Manager.ChangeRenderTargets(pRenderTargetView_, pDepthStencilView_);
}

Direct3DResource* mtgb::Direct3DResource::Clone() const
{
	return new Direct3DResource(*this);
}


