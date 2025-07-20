#include "Quaternion.h"


std::string mtgb::Quaternion::ToString() const
{
	std::string str{ "(" };
	for (int i = 0; i < Quaternion::COUNT; i++)
	{
		str += std::to_string(f[i]);
		if (i < Quaternion::COUNT - 1)
		{
			str += ", ";
		}
	}
	str += ")";

	return str;
}
