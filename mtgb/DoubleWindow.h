#pragma once
#include "ISystem.h"

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
	private:
		WindowRenderContext* context1_;
		WindowRenderContext* context2_;
	};
}