#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"

#include "Direct3DResource.h"
#include "MTAssert.h"
#include "Direct2D/Direct2D.h"
#include "DirectWrite.h"
#include "Game.h"
#include "ISystem.h"
#include <d2d1.h>
#include "WindowContext.h"
#include <map>
#include <typeindex>

typedef struct HWND__* HWND;

namespace mtgb
{

	class Direct2DResource : public WindowContextResource
	{
		friend class DirectWrite;
	public:
		Direct2DResource();
		~Direct2DResource();
		Direct2DResource(const Direct2DResource& other);
		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;
	private:
		ID2D1RenderTarget* pRenderTarget_;
		ID2D1SolidColorBrush* pD2DBrush_;


		// WindowContextResource ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		WindowContextResource* Clone() const override;

	};
}