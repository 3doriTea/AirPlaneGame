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
		/// ���O���t�H�[�}�b�g���ďo�͂���
		/// </summary>
		/// <typeparam name="...Args">�ϒ������^</typeparam>
		/// <param name="_format">�t�H�[�}�b�g������</param>
		/// <param name="..._args">�ϒ�����</param>
		template<typename ...Args>
		static void LogF(const char* _format, const Args... _args);

	private:
		static constexpr size_t BUFFER_SIZE{ 1024 };  // ���O�o�͎��̕�����o�b�t�@�T�C�Y
	};

	template<typename ...Args>
	inline void Debug::LogF(const char* _format, const Args... _args)
	{
		static char buffer[BUFFER_SIZE]{};
		ZeroMemory(buffer, BUFFER_SIZE);  // �k����������

		::sprintf_s<BUFFER_SIZE>(buffer, _format, _args...);

		OutputDebugString(buffer);
	}
}
