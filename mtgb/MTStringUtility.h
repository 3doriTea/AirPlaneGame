#pragma once
#include <string>
#include <string_view>

namespace mtgb
{
	/// <summary>
	/// ワイド文字列に変換する
	/// </summary>
	/// <param name="_cstr"></param>
	/// <returns></returns>
	static inline std::wstring ToWString(const char* _cstr)
	{
		return ToWString(_cstr);
	}

	/// <summary>
	/// ワイド文字列に変換する
	/// </summary>
	/// <param name="_string"></param>
	/// <returns></returns>
	static inline std::wstring ToWString(const std::string& _string)
	{
		return { _string.begin(), _string.end() };
	}

	/// <summary>
	/// ワイド文字列に変換する
	/// </summary>
	/// <param name="_stringView"></param>
	/// <returns></returns>
	static inline std::wstring ToWString(const std::string_view& _stringView)
	{
		return { _stringView.begin(), _stringView.end() };
	}
}
