#include "Matrix4x4.h"
// TODO: #include <sstream> を書くとどこかでctimeがnamespaceグローバル化するエラーがある


std::string mtgb::Matrix4x4::ToString()
{
	std::string str{};
	for (int y = 0; y < ROW_SIZE; y++)
	{
		for (int x = 0; x < COLUMN_SIZE; x++)
		{
			str += std::to_string(r[y].m128_f32[x]);
			if (x < COLUMN_SIZE - 1)
			{
				str += ", ";
			}
		}
		if (y < ROW_SIZE - 1)
		{
			str += "\n";
		}
	}

	return str.c_str();

	//return "";
}
