#pragma once
#include <vector>
#include <map>
#include "ISystem.h"
#include "WindowContext.h"

typedef struct HWND__* HWND;
typedef struct tagMSG MSG;

namespace mtgb
{
	struct WindowConfig
	{
		const wchar_t* title;
		const wchar_t* className;
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

		static HWND CreateWindowContext(const WindowConfig& config);
		void Initialize() override;
		void Update() override;
		void Release();
		
		/// <summary>
		/// WindowContextに対応するWindowConfigを登録
		/// </summary>
		/// <param name="windowContext">ウィンドウの識別子</param>
		/// <param name="config">ウィンドウ設定</param>
		void RegisterWindowConfig(WindowContext windowContext, const WindowConfig& config);
		
		/// <summary>
		/// WindowContextからWindowConfigを取得
		/// </summary>
		/// <param name="windowContext">ウィンドウの識別子</param>
		/// <returns>ウィンドウ設定</returns>
		WindowConfig GetWindowConfig(WindowContext windowContext);
		
	private:
		static MSG* pPeekedMessage_;
		static std::map<WindowContext, WindowConfig> windowConfigMap_;
	};
}