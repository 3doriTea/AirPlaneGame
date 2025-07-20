#include "IShader.h"
#include <d3d11.h>


mtgb::IShader::IShader() :
	pVertexBuffer_ { nullptr },
	pIndexBuffer_{ nullptr },
	pConstantBuffer_{ nullptr }
{
}

void mtgb::IShader::Initialize()
{
	InitializeVertexBuffer(DirectX11Draw::pDevice_);
	InitializeIndexBuffer(DirectX11Draw::pDevice_);
	InitializeConstantBuffer(DirectX11Draw::pDevice_);
}
