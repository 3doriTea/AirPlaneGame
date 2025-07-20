#pragma once
#include "ISystem.h"
#include <Windows.h>
#include <string>

#ifdef _DEBUG

#define LOGF(format, ...) \
	Debug::LogF(format, __VA_ARGS__);

#else

#define LOGF(format, ...)

#endif

namespace mtgb
{
	class Debug : public ISystem
	{
	public:
		Debug();
		~Debug();

		void Initialize() override;
		void Update() override;

	public:
		/// <summary>
		/// ログをフォーマットして出力する
		/// </summary>
		/// <typeparam name="...Args">可変長引数型</typeparam>
		/// <param name="_format">フォーマット文字列</param>
		/// <param name="..._args">可変長引数</param>
		template<typename ...Args>
		static void LogF(const char* _format, const Args... _args);

	private:
		static constexpr size_t BUFFER_SIZE{ 1024 };  // ログ出力時の文字列バッファサイズ
	};

	template<typename ...Args>
	inline void Debug::LogF(const char* _format, const Args... _args)
	{
		static char buffer[BUFFER_SIZE]{};
		ZeroMemory(buffer, BUFFER_SIZE);  // ヌル文字埋め

		::sprintf_s<BUFFER_SIZE>(buffer, _format, _args...);

		OutputDebugString(buffer);
	}
}
