#pragma once
#include "ISystem.h"
#include <vector>
#include <string>
class WindowContext;
typedef struct tagMSG MSG;
typedef struct HWND__* HWND;

namespace mtgb
{

	struct WindowConfig
	{
		std::wstring title;
		std::wstring className;
		int width, height;
		int x, y;
	};

	class WindowManager : public ISystem
	{
	public:
		WindowManager();
		~WindowManager();

		static void CreateWindowRenderContext(const WindowConfig& config, WindowContext ** ppContext);
		void Initialize() override;
		void Update() override;
		void Release();
	private:
		static MSG* pPeekedMessage_;
		//std::vector<WindowRenderContext*> windows_;
		//void GenerateWndClassEx(const WindowConfig& config, WNDCLASSEX* _pWndClassEx);
	};
}