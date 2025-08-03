#pragma once
#include "ISystem.h"
#include "WindowContext.h"
typedef struct HWND__* HWND;

namespace mtgb
{
	class DoubleWindow : public ISystem
	{
	public:
		DoubleWindow();
		~DoubleWindow();

		void Initialize() override;
		void Update() override;

		
		void RegisterFirstWindowCamera();
	private:
		WindowContext context1_;
		WindowContext context2_;
	};
}