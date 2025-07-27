#pragma once
#include "ISystem.h"
#include "MainWindow.h"
typedef struct tagWNDCLASSEXA WNDCLASSEX;
typedef struct HWND__* HWND;
typedef struct tagMSG MSG;

namespace mtgb
{
	class SubWindow : public ISystem
	{
	public:
		SubWindow();
		~SubWindow();

		void Initialize() override;
		void Update() override;

	private:
		MainWindow window_;
		/*/// <summary>
		/// メインウィンドウのハンドラを取得
		/// </summary>
		const HWND GetHWND() const { return hWnd_; }

	private:
		void GenerateWndClassEx(WNDCLASSEX* _pWndClassEx);

	private:
		HWND hWnd_;
		MSG* pPeekedMessage_;*/
	};
}