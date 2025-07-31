#pragma once
#include "ISystem.h"
typedef struct HWND__* HWND;

class WindowContext;
namespace mtgb
{
	class DoubleWindow : public ISystem
	{
	public:
		DoubleWindow();
		~DoubleWindow();

		void Initialize() override;
		void Update() override;

		const HWND GetFirstWindowHandle();
		const HWND GetSecondWindowHandle();
		void ChangeFirstWindow();
		void ChangeSecondWindow();

	private:
		WindowContext* context1_;
		WindowContext* context2_;
	};
}