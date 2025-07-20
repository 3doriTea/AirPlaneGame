#pragma once
#include <string_view>

namespace mtgb
{
	namespace FilePath
	{
		/// <summary>
		/// ファイル名拡張子を取得する
		/// </summary>
		/// <param name="_fileName">ファイル名</param>
		/// <returns>拡張子の文字列</returns>
		inline std::string_view GetExtension(const std::string_view& _fileName)
		{
			size_t extPos{ _fileName.find_last_of('.') };
			return _fileName.substr(extPos, _fileName.size() - extPos);
		}
	}
}
