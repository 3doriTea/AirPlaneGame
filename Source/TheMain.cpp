#include "IncludingWindows.h"
#include <mtgb.h>
#include "SampleGame.h"

// エントリポイント
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	mtgb::Game::Run<SampleGame>();
	return 0x00;
}
