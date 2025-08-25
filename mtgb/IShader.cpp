#include "IShader.h"
#include <d3d11.h>
#include "SceneSystem.h"
#include "ReleaseUtility.h"


mtgb::IShader::IShader() :
	pVertexBuffer_ { nullptr },
	pIndexBuffer_{ nullptr },
	pConstantBuffer_{ nullptr }
{
}

void mtgb::IShader::Initialize()
{
	InitializeVertexBuffer(DirectX11Draw::pDevice_.Get());
	InitializeIndexBuffer(DirectX11Draw::pDevice_.Get());
	InitializeConstantBuffer(DirectX11Draw::pDevice_.Get());
}

void mtgb::IShader::Release()
{
	pVertexBuffer_.Reset();
	pIndexBuffer_.Reset();
	pConstantBuffer_.Reset();
}
