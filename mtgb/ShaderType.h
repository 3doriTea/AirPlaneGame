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
		FbxPartsSkin,
		Unlit3D,
		Debug3D,
		Ground,
		Trail,
		Terrain,
		Max,
	};
}
