#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Handlers.h"
#include <d3d11.h>
namespace mtgb
{
	struct WorldToScreenData
	{
		Matrix4x4 viewMat,projMat;
		D3D11_VIEWPORT viewport;
		uint64_t frameId;
	};
}