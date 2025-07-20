#pragma once
#include <assert.h>
#include <Windows.h>
#include <string>
#include "Game.h"

#if _DEBUG

#define massert(expression)                                                                                                                    \
	if (!(expression))                                                                                                                          \
	{                                                                                                                                          \
		std::string text{ #expression };                                                                                                       \
		text += "\r\n\"";                                                                                                                      \
		text += __FILE__;                                                                                                                      \
		text += "\"より、";                                                                                                                     \
		text += std::to_string(__LINE__);                                                                                                      \
		text += "行目のmassertが起動しました。\r\n";                                                                                                  \
		text += "キャンセルしない場合、スローします。\r\n";                                                                                                  \
		text += "Windowsの最終エラーを取得しますか？(はい/いいえ)";                                                                                                  \
		std::string title{ mtgb::Game::Title() };                                                                                              \
		title += "のアサーションエラー";                                                                                                          \
		int result                                                                                                                             \
		{                                                                                                                                      \
			/*MessageBox(NULL, text.c_str(), title.c_str(), MB_RETRYCANCEL | MB_ICONSTOP | MB_SYSTEMMODAL)*/                                   \
			MessageBox(NULL, text.c_str(), title.c_str(), MB_YESNOCANCEL | MB_ICONSTOP | MB_SYSTEMMODAL)                                       \
		};                                                                                                                                     \
		if (result == IDYES)\
		{\
			DWORD errorCode{ GetLastError() };\
			MessageBox(NULL, (std::string{ "最終エラーコード:" } + std::to_string(errorCode)).c_str(), title.c_str(), MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);\
			throw "this massertion error";                                                                                                     \
		}\
		else if (result == IDNO)\
		{\
			throw "this massertion error";                                                                                                     \
		}\
		else\
		{\
			mtgb::Game::Exit();\
		}\
	}

#else

#define massert(expression) \
	if (!!expression)       \
	{                       \
		mtgb::Game::Exit(); \
	}

#endif
