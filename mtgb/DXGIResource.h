#pragma once
#include "RenderResource.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include <dxgi1_2.h>

typedef struct HWND__* HWND;

namespace mtgb
{
	class DXGIResource : public RenderResource
	{
	public:
		template<typename... Args>
		void Initialize(std::tuple<Args*...>& _resourceTuple, HWND _hWnd);
		void SetResource() override;


		IDXGISwapChain1* pSwapChain1_;
		IDXGIOutput* pOutput_;
		IDXGISurface* pDXGISurface_;
	};
	template<typename ...Args>
	inline void DXGIResource::Initialize(std::tuple<Args*...>& _resourceTuple, HWND _hWnd)
	{
		// DirectX11Manager�ɃA�N�Z�X���ă��\�[�X���쐬
		auto& dx11Manager = Game::System<DirectX11Manager>();

		// �ˑ��֌W�͓��ɂȂ��iDXGIResource�͍ŏ��ɏ����������j
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
		dx11Manager.CreateSwapChain(_hWnd, pOutput_, &pSwapChain1_);

		//�T�[�t�F�X�Ƃ����쐬
		dx11Manager.CreateDXGISurface(pSwapChain1_, &pDXGISurface_);
	}


}