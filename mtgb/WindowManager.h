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

		HWND CreateWindowContext(WindowContext context);
		Vector2Int GetWindowSize(WindowContext context);
		

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
		
		
		//void SwapWindowPos(WindowContext _ctx1 = WindowContext::First, WindowContext _ctx2 = WindowContext::Second);

		static WindowResource& GetWindowResource(WindowContext windowContext);

		/// <summary>
		/// <para> フルスクリーン状態を切り替える </para>
		/// <para> 指定したWindowContextに割り当てられているモニターのサイズに合わせる </para>
		/// </summary>
		/// <param name="_ctx">切り替えるウィンドウ</param>
		static void ChangeFullScreenState(WindowContext _ctx);

		/// <summary>
		/// ウィンドウが一番近い場所にあるモニターでフルスクリーン状態を切り替える
		/// </summary>
		static void ChangeFullScreenStateNearestMonitor(WindowContext _ctx);

		/// <summary>
		/// <para> フルスクリーン状態を切り替える </para>
		/// </summary>
		/// <param name="_ctx">切り替えるウィンドウ</param>
		/// <param name="_rect"></param>
		static void ChangeFullScreenState(WindowContext _ctx, const RECT& _rect);

		static void ResizeWindow(WindowContext _windowContext, UINT _width, UINT _height);
	private:
		static MSG* pPeekedMessage_;
		static std::map<WindowContext, WindowConfig> windowConfigMap_;
	};
}