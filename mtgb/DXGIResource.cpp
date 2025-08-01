#include "DXGIResource.h"

using namespace mtgb;

mtgb::DXGIResource::DXGIResource()
	:pSwapChain1_{nullptr}, pOutput_{nullptr}, pDXGISurface_{nullptr}
{
}

mtgb::DXGIResource::DXGIResource(const DXGIResource& other)
	:WindowContextResource(other)
	,pSwapChain1_{nullptr}
	,pOutput_{nullptr}
	,pDXGISurface_{nullptr}
{
}

void DXGIResource::Initialize(WindowContext _windowContext)
{
	// DirectX11Manager�ɃA�N�Z�X���ă��\�[�X���쐬
	auto& dx11Manager = Game::System<DirectX11Manager>();

	HWND hWnd = Game::System<WindowContextResourceManager>().GetHWND(_windowContext);

	// �}���`���j�^�[�Ή��͍���false�Œ�
	bool isMultiMonitor = false;

	if (isMultiMonitor) {
		// �����I�Ƀ}���`���j�^�[�Ή�����ꍇ��outputIndex���Ǘ�
		int outputIndex = 0; // ���̒l
		dx11Manager.CreateOutput(outputIndex, &pOutput_);
	}
	else {
		pOutput_ = nullptr;
	}

	// �X���b�v�`�F�[�����쐬
	dx11Manager.CreateSwapChain(hWnd, pOutput_, &pSwapChain1_);

	//�T�[�t�F�X�Ƃ����쐬
	dx11Manager.CreateDXGISurface(pSwapChain1_, &pDXGISurface_);
}

void DXGIResource::SetResource()
{
	Game::System<DirectX11Manager>().ChangeSwapChain(pSwapChain1_);
}

WindowContextResource* mtgb::DXGIResource::Clone() const
{
	return new DXGIResource(*this);
}


