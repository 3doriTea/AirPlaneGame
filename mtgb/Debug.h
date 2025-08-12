#pragma once
#include "ISystem.h"
#include "IncludingWindows.h"
#include <string>
#include <chrono>
#include <deque>

#ifdef _DEBUG

#define LOGF(format, ...) \
	mtgb::Debug::LogF(format, __VA_ARGS__)

#define LOGIMGUI(format,...) \
	mtgb::Debug::LogImGui(format,__VA_ARGS__)

#else

#define LOGF(format, ...)

#endif

namespace mtgb
{
	struct LogEntry
	{
		std::string message;
		std::chrono::steady_clock::time_point timestamp;
	};

	class Debug : public ISystem
	{
	public:
		Debug();
		~Debug();

		void Initialize() override;
		void Update() override;

	public:
		/// <summary>
		/// ���O���t�H�[�}�b�g���ďo�͂���
		/// </summary>
		/// <typeparam name="...Args">�ϒ������^</typeparam>
		/// <param name="_format">�t�H�[�}�b�g������</param>
		/// <param name="..._args">�ϒ�����</param>
		template<typename ...Args>
		static void LogF(const char* _format, const Args... _args);

		template<typename ...Args>
		static void LogImGui(const char* _format, const Args..._args);

		static void ShowLogWindow();
	private:
		static constexpr size_t BUFFER_SIZE{ 1024 };  // ���O�o�͎��̕�����o�b�t�@�T�C�Y
		static constexpr UINT MAX_LOG_COUNT{ 30 };

		
		static std::deque<LogEntry> logs_;
	};

	template<typename ...Args>
	inline void Debug::LogF(const char* _format, const Args... _args)
	{
		static char buffer[BUFFER_SIZE]{};
		ZeroMemory(buffer, BUFFER_SIZE);  // �k����������

		::sprintf_s<BUFFER_SIZE>(buffer, _format, _args...);

		OutputDebugString(buffer);
	}
	template<typename ...Args>
	void Debug::LogImGui(const char* _format, const Args ..._args)
	{
		char buffer[BUFFER_SIZE]{};
		ZeroMemory(buffer, BUFFER_SIZE);  // �k����������

		::sprintf_s<BUFFER_SIZE>(buffer, _format, _args...);
		
		logs_.push_front({ buffer,std::chrono::steady_clock::now() });
	}
}
