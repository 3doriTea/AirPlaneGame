#pragma once
#include <dxgi.h>

namespace mtgb
{
	struct MonitorInfo
	{
	int adapterIndex; // 割り当てられているアダプター(GPU)のインデックス
	int outputIndex; // 割り当てられているモニターのインデックス
	bool isRequested; // 使用済みかどうかのフラグ
	DXGI_OUTPUT_DESC desc;
	};
}
