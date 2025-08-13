#include "DirectX11Manager.h"
#include "Game.h"
#include "MTAssert.h"
#include "IncludingWindows.h"
#include <d3d11.h>
#include <dxgi1_2.h> 
#include <DirectXMath.h>
#include "DirectX11Draw.h"
#include "MTImGui.h"
#include "MainWindow.h"
#include "Screen.h"
#include "Vector3.h"
#include <d3dcompiler.h>
#include "HLSLInclude.h"
#include "WindowContext.h"
#include "ReleaseUtility.h"

mtgb::DirectX11Manager::DirectX11Manager()
{
}

mtgb::DirectX11Manager::~DirectX11Manager()
{
}

void mtgb::DirectX11Manager::Initialize()
{
#if 0
	HRESULT hResult{};

	STARTUPINFO startupInfo{};
	GetStartupInfo(&startupInfo);
	int nCmdShow = startupInfo.wShowWindow;

	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	HWND hMainWindow{ Game::System<MainWindow>().GetHWND() };

	// �_�u���o�b�t�@�����O�̐؂�ւ����̐ݒ�
	DXGI_SWAP_CHAIN_DESC swapChainDesc
	{
		.BufferDesc
		{
			.Width = static_cast<UINT>(SCREEN_SIZE.x),
			.Height = static_cast<UINT>(SCREEN_SIZE.y),
			.RefreshRate
			{
				.Numerator = 60,  // ���t���b�V�����[�g�̕���
				.Denominator = 1,  // ���t���b�V�����[�g�̕��q
			},
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,  // �g����F��
		},
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 1,  // ����ʂ̖���
		.OutputWindow = hMainWindow,
		.Windowed = TRUE,  // �E�B���h�E���[�h��
	};

	D3D_FEATURE_LEVEL level{};
	hResult = D3D11CreateDeviceAndSwapChain(
		nullptr,                      // �r�f�I�A�_�v�^�[�̎w��
		D3D_DRIVER_TYPE_HARDWARE,     // �h���C�o�̃^�C�v (�n�[�h�E�F�A���\�t�g�E�F�A��)
		nullptr,                      // �h���C�o�̃^�C�v���\�t�g�E�F�A�̏ꍇ�Ɏw�肷�郂�W���[��
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,    // �t���O (�f�o�b�O�p�ɂ���),Direct2D�g����悤�ɂ���
		nullptr,                      // �f�o�C�X�A�R���e�L�X�g�̃��x���ݒ� (nullptr�ōō����x��)
		0,                            // ���x���ݒ�̐�
		D3D11_SDK_VERSION,            // DirectX11��SDK�o�[�W����
		&swapChainDesc,               // �X���b�v�`�F�[���̐ݒ�
		&DirectX11Draw::pSwapChain_,  // �쐬����X���b�v�`�F�[���̃|�C���^�n��
		&DirectX11Draw::pDevice_,     // �쐬����f�o�C�X�̃|�C���^�n��
		&level,                       // �쐬���ꂽ�f�o�C�X&&�R���e�L�X�g�̃��x�����̃|�C���^�n��
		&DirectX11Draw::pContext_);   // �􂷂�R���e�L�X�g�̃|�C���^�n��

	massert(SUCCEEDED(hResult)  // D3D11CreateDeviceAndSwapChain�ɐ������Ă���
		&& "D3D11CreateDeviceAndSwapChain�Ɏ��s @DirectX11Manager::Initialize");

	ID3D11Texture2D* pBackBuffer{ nullptr };
	hResult = DirectX11Draw::pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));

	massert(SUCCEEDED(hResult)  // �X���b�v�`�F�[�������0�Ԗڃo�b�t�@�擾�ɐ������Ă���
		&& "�X���b�v�`�F�[�������0�Ԗڃo�b�t�@�擾�Ɏ��s @DirectX11Manager::Initialize");

	hResult = DirectX11Draw::pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &DirectX11Draw::pRenderTargetView_);

	massert(SUCCEEDED(hResult)  // �`���̍쐬�ɐ������Ă���
		&& "�`���̍쐬�Ɏ��s @DirectX11Manager::Initialize");

	pBackBuffer->Release();  // �o�b�N�o�b�t�@�͎g��Ȃ����߉������

	D3D11_VIEWPORT viewport  // �`���ʂ̐ݒ�
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = static_cast<float>(SCREEN_SIZE.x),
		.Height = static_cast<float>(SCREEN_SIZE.y),
		.MinDepth = 0,
		.MaxDepth = 1,
	};

	DirectX11Draw::pContext_->RSSetViewports(
		1,           // �Z�b�g����r���[�|�[�g�̐� 1����
		&viewport);  // �Z�b�g����r���[�|�[�g (�z��\)

	InitializeShaderBundle();  // �V�F�[�_�o���h���̏�����

	DirectX11Draw::SetShader(ShaderType::FbxParts);

#pragma region �[�x�o�b�t�@�쐬
	// �[�x�o�b�t�@�̐ݒ�
	const D3D11_TEXTURE2D_DESC DEPTH_TEXTURE2D_DESC
	{
		.Width = static_cast<UINT>(SCREEN_SIZE.x),
		.Height = static_cast<UINT>(SCREEN_SIZE.y),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_D32_FLOAT,
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0
		},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
	};

	hResult = DirectX11Draw::pDevice_->CreateTexture2D(
		&DEPTH_TEXTURE2D_DESC,
		nullptr,
		&DirectX11Draw::pDepthStencil_);

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���o�b�t�@�̍쐬�Ɏ��s
		&& "�[�x�X�e���V���o�b�t�@�̍쐬�Ɏ��s");

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilView(
		DirectX11Draw::pDepthStencil_,
		nullptr,
		&DirectX11Draw::pDepthStencilView_);

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���r���̍쐬�ɐ���
		&& "�[�x�X�e���V���r���̍쐬�Ɏ��s");

	// �[�x�X�e���V����
	const D3D11_DEPTH_STENCIL_DESC DEPTH_STENCIL_DESC
	{
		.DepthEnable = TRUE,
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO,
		.DepthFunc = D3D11_COMPARISON_LESS_EQUAL,
		.StencilEnable = TRUE,
		.StencilReadMask = {},
		.StencilWriteMask = {},
		.FrontFace
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS,
		},
		.BackFace
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS,
		}
	};

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilState(
		&DEPTH_STENCIL_DESC,
		&DirectX11Draw::pDepthStencilState_[static_cast<size_t>(BlendMode::Default)]);

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���X�e�[�g�̍쐬�ɐ���
		&& "�[�x�X�e���V���X�e�[�g�̍쐬�Ɏ��s");

	DirectX11Draw::pContext_->OMSetDepthStencilState(
		DirectX11Draw::pDepthStencilState_[static_cast<size_t>(BlendMode::Default)],
		0);
#pragma endregion

#pragma region �u�����h�X�e�[�g�쐬
	const D3D11_BLEND_DESC BLEND_DESC
	{
		.AlphaToCoverageEnable = FALSE,
		.IndependentBlendEnable = FALSE,
		.RenderTarget
		{
			D3D11_RENDER_TARGET_BLEND_DESC
			{
				.BlendEnable = TRUE,
				.SrcBlend = D3D11_BLEND_SRC_ALPHA,
				.DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
				.BlendOp = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D11_BLEND_ONE,
				.DestBlendAlpha = D3D11_BLEND_ZERO,
				.BlendOpAlpha = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
			},
		},
	};

	hResult = DirectX11Draw::pDevice_->CreateBlendState(
		&BLEND_DESC,
		&DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::Default)]);

	massert(SUCCEEDED(hResult)  // �u�����h�X�e�[�g�쐬�ɐ���
		&& "�u�����h�X�e�[�g�쐬�Ɏ��s");

	float blendFactor[4]{ D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	DirectX11Draw::pContext_->OMSetBlendState(
		DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::Default)],
		blendFactor,
		0xffffffffU);
#pragma endregion

	//�f�t�H���g�̃T���v���[
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	DirectX11Draw::pDevice_->CreateSamplerState(&desc, &DirectX11Draw::pDefaultSamplerState_);

	// �e�N�X�`�����Ȃ��ꍇ�ł�
	DirectX11Draw::pContext_->PSSetSamplers(0, 1, &DirectX11Draw::pDefaultSamplerState_);

	DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectX11Draw::pContext_->OMSetRenderTargets(
		1,
		&DirectX11Draw::pRenderTargetView_,
		DirectX11Draw::pDepthStencilView_);
#endif
	InitializeCommonResources();
}

void mtgb::DirectX11Manager::Update()
{
	/*Game::System<MTImGui>().EndFrame();
	DirectX11Draw::End();
	

	Game::System<MTImGui>().BeginFrame();
	DirectX11Draw::Begin();*/
}

void mtgb::DirectX11Manager::InitializeCommonResources()
{
	HRESULT hResult{};

	STARTUPINFO startupInfo{};
	GetStartupInfo(&startupInfo);
	int nCmdShow = startupInfo.wShowWindow;

	
	D3D_FEATURE_LEVEL level{};

	hResult = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&DirectX11Draw::pDevice_,
		&level,
		&DirectX11Draw::pContext_
	);
	massert(SUCCEEDED(hResult)
	 && "D3D11CreateDevice�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	hResult = DirectX11Draw::pDevice_->QueryInterface(_uuidof(IDXGIDevice1), (void**)&(DirectX11Draw::pDXGIDevice_));

	massert(SUCCEEDED(hResult)
		&& "QueryInterface�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	hResult = DirectX11Draw::pDXGIDevice_->GetAdapter(&(DirectX11Draw::pDXGIAdapter_));
	massert(SUCCEEDED(hResult)
		&& "GetAdapter�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	hResult = DirectX11Draw::pDXGIAdapter_->GetParent(__uuidof(IDXGIFactory2), (void**)&(DirectX11Draw::pDXGIFactory_));
	massert(SUCCEEDED(hResult)
		&& "GetParent�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	InitializeShaderBundle();  // �V�F�[�_�o���h���̏�����

	const D3D11_SAMPLER_DESC SAMPLER_DESC
	{
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
	};

	hResult = DirectX11Draw::pDevice_->CreateSamplerState(&SAMPLER_DESC, DirectX11Draw::pDefaultSamplerState_.GetAddressOf());
	massert(SUCCEEDED(hResult)
		&& "�f�t�H���g�̃T���v���쐬�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	// �[�x�X�e���V����
	const D3D11_DEPTH_STENCIL_DESC DEPTH_STENCIL_DESC
	{
		.DepthEnable = TRUE,
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		.DepthFunc = D3D11_COMPARISON_LESS_EQUAL,
		.StencilEnable = TRUE,
		.StencilReadMask = {},
		.StencilWriteMask = {},
		.FrontFace
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS,
		},
		.BackFace
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS,
		}
	};

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilState(
		&DEPTH_STENCIL_DESC,
		&DirectX11Draw::pDepthStencilState_[static_cast<size_t>(BlendMode::Default)]);

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���X�e�[�g�̍쐬�ɐ���
		&& "�[�x�X�e���V���X�e�[�g�̍쐬�Ɏ��s");

#pragma region �u�����h�X�e�[�g�쐬
	const D3D11_BLEND_DESC BLEND_DESC
	{
		.AlphaToCoverageEnable = FALSE,
		.IndependentBlendEnable = FALSE,
		.RenderTarget
		{
			D3D11_RENDER_TARGET_BLEND_DESC
			{
				.BlendEnable = TRUE,
				.SrcBlend = D3D11_BLEND_SRC_ALPHA,
				.DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
				.BlendOp = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D11_BLEND_ONE,
				.DestBlendAlpha = D3D11_BLEND_ZERO,
				.BlendOpAlpha = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
			},
		},
	};

	hResult = DirectX11Draw::pDevice_->CreateBlendState(
		&BLEND_DESC,
		&DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::Default)]);

	massert(SUCCEEDED(hResult)  // �u�����h�X�e�[�g�쐬�ɐ���
		&& "�u�����h�X�e�[�g�쐬�Ɏ��s");
#pragma endregion
	DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void mtgb::DirectX11Manager::CreateDXGISurface(IDXGISwapChain1* pSwapChain1, IDXGISurface** ppDXGISurface)
{
	HRESULT hResult{};
	
	//�o�b�N�o�b�t�@�󂯎��
	ID3D11Texture2D* pBackBuffer = nullptr;
	hResult = pSwapChain1->GetBuffer(0, _uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	massert(SUCCEEDED(hResult)
		&& "GetBuffer�Ɏ��s @DirectX11Manager::CreateDXGISurface");

	//�o�b�N�o�b�t�@����IDXGISurface�C���^�[�t�F�[�X�����o��
	hResult = pBackBuffer->QueryInterface(IID_PPV_ARGS(ppDXGISurface));

	SAFE_RELEASE(pBackBuffer);

	massert(SUCCEEDED(hResult)
		&& "QueryInterface�Ɏ��s @DirectX11Manager::CreateDXGISurface");
}

void mtgb::DirectX11Manager::CreateOutput(int index, IDXGIOutput** ppOutput)
{
	HRESULT hResult{};
	
	hResult = DirectX11Draw::pDXGIAdapter_->EnumOutputs(index, ppOutput);
	massert(SUCCEEDED(hResult)
		&& "EnumOutputs�Ɏ��s @DirectX11Manager::CreateOutput");
}

void mtgb::DirectX11Manager::CreateSwapChain(HWND hWnd, IDXGIOutput* pOutput, IDXGISwapChain1** ppSwapChain1)
{
	HRESULT hResult{};
	
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	
	DXGI_SWAP_CHAIN_DESC1 desc
	{
		.Width = static_cast<UINT>(SCREEN_SIZE.x),//�𑜓x(�s�N�Z����)�B0�Ȃ�E�B���h�E�̃T�C�Y�ɍ��킹��
		.Height = static_cast<UINT>(SCREEN_SIZE.y),//�𑜓x(�s�N�Z����)�B0�Ȃ�E�B���h�E�̃T�C�Y�ɍ��킹��
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,  // �g����F��
		.Stereo = FALSE,//�X�e���I(3D���̎�)�\����L���ɂ��邩
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,  // ����ʂ̖���
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags = 0
	};

	hResult = DirectX11Draw::pDXGIFactory_->CreateSwapChainForHwnd(
		DirectX11Draw::pDevice_.Get(),
		hWnd,
		&desc,
		nullptr,//�t���X�N���[���̐ݒ�
		pOutput,//�o��
		ppSwapChain1
	);
	massert(SUCCEEDED(hResult)
		&& "CreateSwapChainForHwnd�Ɏ��s @DirectX11Manager::CreateSwapChain");
}

void mtgb::DirectX11Manager::CreateRenderTargetView(IDXGISwapChain1* pSwapChain1, ID3D11RenderTargetView** ppRenderTargetView)
{
	HRESULT hResult{};
	
	ID3D11Texture2D* pBackBuffer{ nullptr };
	hResult = pSwapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	massert(SUCCEEDED(hResult) 
		&& "GetBuffer�Ɏ��s @DirectX11Manager::CreateRenderTargetView");

	hResult = DirectX11Draw::pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, ppRenderTargetView);
	massert(SUCCEEDED(hResult)
		&& "CreateRenderTargetView�Ɏ��s @DirectX11Manager::CreateRenderTargetView");

	pBackBuffer->Release();  // �o�b�N�o�b�t�@�͎g��Ȃ����߉������
}

void mtgb::DirectX11Manager::CreateViewport(D3D11_VIEWPORT& viewport)
{
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	
	viewport =
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = static_cast<float>(SCREEN_SIZE.x),
		.Height = static_cast<float>(SCREEN_SIZE.y),
		.MinDepth = 0,
		.MaxDepth = 1,
	};
}

void mtgb::DirectX11Manager::CreateDepthStencilAndDepthStencilView(ID3D11Texture2D** ppDepthStencil, ID3D11DepthStencilView** ppDepthStencilView)
{
	HRESULT hResult{};
	
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	
	// �[�x�o�b�t�@�̐ݒ�
	const D3D11_TEXTURE2D_DESC DEPTH_TEXTURE2D_DESC
	{
		.Width = static_cast<UINT>(SCREEN_SIZE.x),
		.Height = static_cast<UINT>(SCREEN_SIZE.y),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_D32_FLOAT,
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0
		},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
	};

	hResult = DirectX11Draw::pDevice_->CreateTexture2D(
		&DEPTH_TEXTURE2D_DESC,
		nullptr,
		ppDepthStencil);

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���o�b�t�@�̍쐬�Ɏ��s
		&& "�[�x�X�e���V���o�b�t�@�̍쐬�Ɏ��s");

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilView(
		*ppDepthStencil,
		nullptr,
		ppDepthStencilView);

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���r���̍쐬�ɐ���
		&& "�[�x�X�e���V���r���[�̍쐬�Ɏ��s");
}



void mtgb::DirectX11Manager::ChangeViewport(const D3D11_VIEWPORT& viewport)
{
	DirectX11Draw::pContext_->RSSetViewports(1, &viewport);
}

void mtgb::DirectX11Manager::ChangeRenderTargets(ComPtr<ID3D11RenderTargetView> pRenderTargetView, ComPtr<ID3D11DepthStencilView> pDepthStencilView)
{
	DirectX11Draw::pRenderTargetView_ = pRenderTargetView;
	DirectX11Draw::pDepthStencilView_ = pDepthStencilView;

	DirectX11Draw::pContext_->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());
}

void mtgb::DirectX11Manager::ChangeSwapChain(ComPtr<IDXGISwapChain1> pSwapChain1)
{
	DirectX11Draw::pSwapChain1_ = pSwapChain1;
}

void mtgb::DirectX11Manager::Release()
{
	DirectX11Draw::Release();
}

void mtgb::DirectX11Manager::InitializeShaderBundle()
{
	DWORD vectorSize{ sizeof(Vector3) };

	CD3D11_RASTERIZER_DESC cRasterizerDesc{};

	// 2D���ʂ̃C���v�b�g���C�A�E�g
	const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_DESC_2D[]
	{
		{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 1,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
	};

	// 3D���ʂ̃C���v�b�g���C�A�E�g
	const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_DESC_3D[]
	{
		{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 1,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 2,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
	};

	// 2D�}�`�p�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
		{
			.FillMode = D3D11_FILL_SOLID,  // �h��Ԃ�: solid
			.CullMode = D3D11_CULL_BACK,  // �J�����O: �A�ʏ���
			.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
			.DepthBias = {},
			.DepthBiasClamp = {},
			.SlopeScaledDepthBias = {},
			.DepthClipEnable = true,//�N���b�s���O��L���ɂ���
			.ScissorEnable = {},
			.MultisampleEnable = {},
			.AntialiasedLineEnable = {},
		});

		CompileShader(
			L"Shader/Figure.hlsl",
			ShaderType::Figure,
			INPUT_ELEMENT_DESC_2D,
			sizeof(INPUT_ELEMENT_DESC_2D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// 2D�X�v���C�g�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,  // �h��Ԃ�: solid
				.CullMode = D3D11_CULL_NONE,  // �J�����O: �A�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,//�N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Sprite.hlsl",
			ShaderType::Sprite2D,
			INPUT_ELEMENT_DESC_2D,
			sizeof(INPUT_ELEMENT_DESC_2D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// FbxParts�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,  // �h��Ԃ�: solid
				.CullMode = D3D11_CULL_BACK,  // �J�����O: �A�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,//�N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/FbxParts.hlsl",
			ShaderType::FbxParts,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// Unlit3D�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,  // �h��Ԃ�: solid
				.CullMode = D3D11_CULL_BACK,  // �J�����O: �A�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,//�N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Unlit3D.hlsl",
			ShaderType::Unlit3D,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// Debug3D�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_WIREFRAME,  // �g����: wireframe
				.CullMode = D3D11_CULL_NONE,  // �J�����O: �B�ʏ������Ȃ�
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,//�N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Debug3D.hlsl",
			ShaderType::Debug3D,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}
}

void mtgb::DirectX11Manager::CompileShader(
	const std::wstring& _fileName,
	const ShaderType& _type,
	const D3D11_INPUT_ELEMENT_DESC* _pHLSLLayout,
	const unsigned int _layoutLength,
	const CD3D11_RASTERIZER_DESC* _pRasterizerDesc)
{
	HLSLInclude hlslInclude{};
	HRESULT hResult{};

#pragma region ���_�V�F�[�_
	// ���_�V�F�[�_�̃C���^�t�F�[�X
	ID3DBlob* pCompileVS{ nullptr };

	// ���_�V�F�[�_�̃R���p�C��
	hResult = D3DCompileFromFile(
		_fileName.c_str(),  // �t�@�C���p�X
		nullptr,            // �V�F�[�_�}�N���̔z��
		&hlslInclude,       // �C���N���[�h������
		"VS",               // �G���g���|�C���g�̊֐���
		"vs_5_0",           // �V�F�[�_�̃o�[�W���� (�I�v�V�����ŕt������)
		0,                  // �I�v�V�����t���O1
		0,                  // �I�v�V�����t���O2
		&pCompileVS,        // �R���p�C���ς݃R�[�h�ւ̃A�N�Z�X�C���^�t�F�[�X
		nullptr);           // �G���[���b�Z�[�W��M�p ����

	massert(SUCCEEDED(hResult)  // ���_�V�F�[�_�̃R���p�C���ɐ���
		&& "���_�V�F�[�_�̃R���p�C���Ɏ��s @DirectX11Manager::CompileShader");

	// ���_�V�F�[�_���쐬���A�w��^�C�v�̃o���h���Ɋi�[����
	hResult = DirectX11Draw::pDevice_->CreateVertexShader(
		pCompileVS->GetBufferPointer(),  // �R���p�C�����ꂽ�o�b�t�@�̃|�C���^
		pCompileVS->GetBufferSize(),     // �o�b�t�@�̃T�C�Y
		nullptr,                         // �����P�[�W�N���X: ����
		&DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pVertexShader);

	massert(SUCCEEDED(hResult)  // ���_�V�F�[�_�̍쐬�ɐ���
		&& "���_�V�F�[�_�̍쐬�Ɏ��s @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region �s�N�Z���V�F�[�_
	// �s�N�Z���V�F�[�_�̃C���^�t�F�[�X
	ID3DBlob* pCompilePS{ nullptr };

	// �s�N�Z���V�F�[�_�̃R���p�C��
	hResult = D3DCompileFromFile(
		_fileName.c_str(),  // �t�@�C���p�X
		nullptr,            // �V�F�[�_�}�N���̔z��
		&hlslInclude,       // �C���N���[�h������
		"PS",               // �G���g���|�C���g�̊֐���
		"ps_5_0",           // �V�F�[�_�̃o�[�W���� (�I�v�V�����ŕt������)
		0,                  // �I�v�V�����t���O1
		0,                  // �I�v�V�����t���O2
		&pCompilePS,        // �R���p�C���ς݃R�[�h�ւ̃A�N�Z�X�C���^�t�F�[�X
		nullptr);           // �G���[���b�Z�[�W��M�p ����

	massert(SUCCEEDED(hResult)  // �s�N�Z���V�F�[�_�̃R���p�C���ɐ���
		&& "�s�N�Z���V�F�[�_�̃R���p�C���Ɏ��s @DirectX11Manager::CompileShader");

	// �s�N�Z���V�F�[�_���쐬���A�w��^�C�v�̃o���h���Ɋi�[����
	hResult = DirectX11Draw::pDevice_->CreatePixelShader(
		pCompilePS->GetBufferPointer(),  // �R���p�C�����ꂽ�o�b�t�@�̃|�C���^
		pCompilePS->GetBufferSize(),     // �o�b�t�@�̃T�C�Y
		nullptr,                         // �����P�[�W�N���X: ����
		&DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pPixelShader);

	massert(SUCCEEDED(hResult)  // �s�N�Z���V�F�[�_�̍쐬�ɐ���
		&& "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region ���_���C�A�E�g
	// ���_���C�A�E�g���쐬���A�w��^�C�v�̃o���h���Ɋi�[����
	hResult = DirectX11Draw::pDevice_->CreateInputLayout(
		_pHLSLLayout,                    // ���̓f�[�^�^�z��
		_layoutLength,                   // ���̓f�[�^�^�z��̗v�f��
		pCompileVS->GetBufferPointer(),  // �R���p�C�����ꂽ�o�b�t�@�̃|�C���^
		pCompileVS->GetBufferSize(),     // �o�b�t�@�̃T�C�Y
		&DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pVertexLayout);

	massert(SUCCEEDED(hResult)  // ���_���C�A�E�g�̍쐬�ɐ���
		&& "���_���C�A�E�g�̍쐬�Ɏ��s @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region ���X�^���C�U
	// ���X�^���C�U���쐬���A�w��^�C�v�̃o���h���Ɋi�[����
	DirectX11Draw::pDevice_->CreateRasterizerState(
		_pRasterizerDesc,  // ���X�^���C�U�̐ݒ�
		&DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pRasterizerState);

	massert(SUCCEEDED(hResult)  // ���X�^���C�U�̍쐬�ɐ���
		&& "���X�^���C�U�̍쐬�Ɏ��s @DirectX11Manager::CompileShader");
#pragma endregion


	// ������Ă���
	pCompileVS->Release();
	pCompilePS->Release();
}
