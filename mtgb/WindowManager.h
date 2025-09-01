#pragma once
#include <vector>
#include <map>
#include <string>
#include "ISystem.h"
#include "WindowContext.h"
#include "Vector2Int.h"
typedef struct HWND__* HWND;
typedef struct tagMSG MSG;

namespace mtgb
{
	class WindowResource;
	struct WindowConfig
	{
		std::string title;
		std::string className;
		int width;
		int height;
		int x;
		int y;
	};

	class WindowManager : public ISystem
	{
	public:
		WindowManager();
		~WindowManager();

		static HWND CreateWindowContext(WindowContext context);
		static Vector2Int GetWindowSize(WindowContext context);
		

		void Initialize() override;
		void Update() override;
		void Release();
		
		/// <summary>
		/// WindowContextに対応するWindowConfigをセット
		/// </summary>
		/// <param name="windowContext">ウィンドウの識別子</param>
		/// <param name="config">ウィンドウ設定</param>
		void SetWindowConfig(WindowContext windowContext, const WindowConfig& config);
		
		
		/// <summary>
		/// WindowContextからWindowConfigを取得
		/// </summary>
		/// <param name="windowContext">ウィンドウの識別子</param>
		/// <returns>ウィンドウ設定</returns>
		static WindowConfig GetWindowConfig(WindowContext windowContext);
		

		static WindowResource& GetWindowResource(WindowContext windowContext);


		static void ResizeWindow(WindowContext _windowContext, UINT _width, UINT _height);
	private:
		static MSG* pPeekedMessage_;
		static std::map<WindowContext, WindowConfig> windowConfigMap_;
	};
}