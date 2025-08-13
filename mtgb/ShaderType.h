#pragma once
#include <cmtgb.h>

namespace mtgb
{
	/// <summary>
	/// シェーダタイプ
	/// </summary>
	enum struct ShaderType : int8_t
	{
		Figure,
		Sprite2D,
		FbxParts,
		Unlit3D,
		Debug3D,
		Ground,
		Max,
	};
}
