#pragma once
#include <cstdint>
#include <cassert>

namespace mtbin
{
	/// <summary>
	/// mtbinのバイト型
	/// </summary>
	using Byte = unsigned char;

	namespace Utility
	{
		/// <summary>
		/// チャンクIdを比較する
		/// </summary>
		/// <typeparam name="SIZE">Idのバイト数</typeparam>
		/// <param name="_header">チャンクId格納配列</param>
		/// <param name="_str">比較Id文字列</param>
		/// <returns>合致 true / false</returns>
		template<size_t SIZE>
		inline bool CompareId(const Byte _header[SIZE], const char* _str)
		{
			for (size_t i = 0; i < SIZE; i++)
			{
				if (_header[i] != _str[i])
				{
					return false;
				}
			}
			return true;
		}
	}
}
