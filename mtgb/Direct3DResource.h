#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"
#include "DXGIResource.h"
#include "MTAssert.h"
#include "WindowContext.h"
#include <map>
#include <typeindex>

namespace mtgb
{

	class Direct3DResource : public WindowContextResource
	{
	public:
		Direct3DResource();
		~Direct3DResource();
		Direct3DResource(const Direct3DResource& other);
		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;
	private:
		D3D11_VIEWPORT viewPort_;
		ID3D11RenderTargetView* pRenderTargetView_;
		ID3D11Texture2D* pDepthStencil_;
		ID3D11DepthStencilView* pDepthStencilView_;


		// WindowContextResource ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		Direct3DResource* Clone() const override;

	};
}