#include "DXGIResource.h"
#include "ReleaseUtility.h"
#include "WindowContextUtil.h"
using namespace mtgb;

mtgb::DXGIResource::DXGIResource()
	:pSwapChain1_{nullptr}, pOutput_{nullptr}, pDXGISurface_{nullptr}
{
}

mtgb::DXGIResource::~DXGIResource()
{
	pSwapChain1_.Reset();
	pOutput_.Reset();
	pDXGISurface_.Reset();
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

	HWND hWnd = WinCtxRes::GetHWND(_windowContext);

	// �}���`���j�^�[�Ή��͍���false�Œ�
	bool isMultiMonitor = false;

	if (isMultiMonitor) {
		// �����I�Ƀ}���`���j�^�[�Ή�����ꍇ��outputIndex���Ǘ�
		int outputIndex = 0; // ���̒l
		IDXGIOutput* pRawDXGIOutput = nullptr;
		dx11Manager.CreateOutput(outputIndex, &pRawDXGIOutput);
		pOutput_.Attach(pRawDXGIOutput);
	}
	else {
		pOutput_ = nullptr;
	}

	IDXGISwapChain1* pRawSwapChain1 = nullptr;
	// �X���b�v�`�F�[�����쐬
	dx11Manager.CreateSwapChain(hWnd, pOutput_.Get(), &pRawSwapChain1);
	pSwapChain1_.Attach(pRawSwapChain1);

	IDXGISurface* pRawSurface = nullptr;
	//�T�[�t�F�X�Ƃ����쐬
	dx11Manager.CreateDXGISurface(pSwapChain1_.Get(), &pRawSurface);
	pDXGISurface_.Attach(pRawSurface);

}

void DXGIResource::SetResource()
{
	Game::System<DirectX11Manager>().ChangeSwapChain(pSwapChain1_);
}

WindowContextResource* mtgb::DXGIResource::Clone() const
{
	return new DXGIResource(*this);
}


