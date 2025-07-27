#pragma once
#include "ISystem.h"
#include <vector>
#include <string>
class WindowRenderContext;
struct WNDCLASSEX;
typedef struct tagMSG MSG;
namespace mtgb
{

	struct WindowConfig
	{
		std::wstring title;
		int width, height;
		int x, y;
	};

	class WindowManager
	{
	public:
		WindowManager();
		~WindowManager();
		static void CreateWindowRenderContext(const WindowConfig& config, WindowRenderContext ** ppContext);
		void Update();
		void Release();
	private:
		static MSG* pPeekedMessage_;
		//std::vector<WindowRenderContext*> windows_;
		//void GenerateWndClassEx(const WindowConfig& config, WNDCLASSEX* _pWndClassEx);
	};
}