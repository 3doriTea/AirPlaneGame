#pragma once
#include "ISystem.h"
typedef struct HWND__* HWND;

class WindowRenderContext;
namespace mtgb
{
	class DoubleWindow : public ISystem
	{
	public:
		DoubleWindow();
		~DoubleWindow();

		void Initialize() override;
		void Update() override;

		const HWND GetHWND();

	private:
		WindowRenderContext* context1_;
		WindowRenderContext* context2_;
	};
}