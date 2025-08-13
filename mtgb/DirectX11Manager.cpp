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

	// ダブルバッファリングの切り替え役の設定
	DXGI_SWAP_CHAIN_DESC swapChainDesc
	{
		.BufferDesc
		{
			.Width = static_cast<UINT>(SCREEN_SIZE.x),
			.Height = static_cast<UINT>(SCREEN_SIZE.y),
			.RefreshRate
			{
				.Numerator = 60,  // リフレッシュレートの分母
				.Denominator = 1,  // リフレッシュレートの分子
			},
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,  // 使える色数
		},
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 1,  // 裏画面の枚数
		.OutputWindow = hMainWindow,
		.Windowed = TRUE,  // ウィンドウモードか
	};

	D3D_FEATURE_LEVEL level{};
	hResult = D3D11CreateDeviceAndSwapChain(
		nullptr,                      // ビデオアダプターの指定
		D3D_DRIVER_TYPE_HARDWARE,     // ドライバのタイプ (ハードウェアかソフトウェアか)
		nullptr,                      // ドライバのタイプがソフトウェアの場合に指定するモジュール
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,    // フラグ (デバッグ用にする),Direct2D使えるようにする
		nullptr,                      // デバイス、コンテキストのレベル設定 (nullptrで最高レベル)
		0,                            // レベル設定の数
		D3D11_SDK_VERSION,            // DirectX11のSDKバージョン
		&swapChainDesc,               // スワップチェーンの設定
		&DirectX11Draw::pSwapChain_,  // 作成するスワップチェーンのポインタ渡し
		&DirectX11Draw::pDevice_,     // 作成するデバイスのポインタ渡し
		&level,                       // 作成されたデバイス&&コンテキストのレベル情報のポインタ渡し
		&DirectX11Draw::pContext_);   // 策するコンテキストのポインタ渡し

	massert(SUCCEEDED(hResult)  // D3D11CreateDeviceAndSwapChainに成功している
		&& "D3D11CreateDeviceAndSwapChainに失敗 @DirectX11Manager::Initialize");

	ID3D11Texture2D* pBackBuffer{ nullptr };
	hResult = DirectX11Draw::pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));

	massert(SUCCEEDED(hResult)  // スワップチェーンからの0番目バッファ取得に成功している
		&& "スワップチェーンからの0番目バッファ取得に失敗 @DirectX11Manager::Initialize");

	hResult = DirectX11Draw::pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &DirectX11Draw::pRenderTargetView_);

	massert(SUCCEEDED(hResult)  // 描画先の作成に成功している
		&& "描画先の作成に失敗 @DirectX11Manager::Initialize");

	pBackBuffer->Release();  // バックバッファは使わないため解放する

	D3D11_VIEWPORT viewport  // 描画画面の設定
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = static_cast<float>(SCREEN_SIZE.x),
		.Height = static_cast<float>(SCREEN_SIZE.y),
		.MinDepth = 0,
		.MaxDepth = 1,
	};

	DirectX11Draw::pContext_->RSSetViewports(
		1,           // セットするビューポートの数 1つだけ
		&viewport);  // セットするビューポート (配列可能)

	InitializeShaderBundle();  // シェーダバンドルの初期化

	DirectX11Draw::SetShader(ShaderType::FbxParts);

#pragma region 深度バッファ作成
	// 深度バッファの設定
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

	massert(SUCCEEDED(hResult)  // 深度ステンシルバッファの作成に失敗
		&& "深度ステンシルバッファの作成に失敗");

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilView(
		DirectX11Draw::pDepthStencil_,
		nullptr,
		&DirectX11Draw::pDepthStencilView_);

	massert(SUCCEEDED(hResult)  // 深度ステンシルビュの作成に成功
		&& "深度ステンシルビュの作成に失敗");

	// 深度ステンシルの
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

	massert(SUCCEEDED(hResult)  // 深度ステンシルステートの作成に成功
		&& "深度ステンシルステートの作成に失敗");

	DirectX11Draw::pContext_->OMSetDepthStencilState(
		DirectX11Draw::pDepthStencilState_[static_cast<size_t>(BlendMode::Default)],
		0);
#pragma endregion

#pragma region ブレンドステート作成
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

	massert(SUCCEEDED(hResult)  // ブレンドステート作成に成功
		&& "ブレンドステート作成に失敗");

	float blendFactor[4]{ D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	DirectX11Draw::pContext_->OMSetBlendState(
		DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::Default)],
		blendFactor,
		0xffffffffU);
#pragma endregion

	//デフォルトのサンプラー
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	DirectX11Draw::pDevice_->CreateSamplerState(&desc, &DirectX11Draw::pDefaultSamplerState_);

	// テクスチャがない場合でも
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
	 && "D3D11CreateDeviceに失敗 @DirectX11Manager::InitializeCommonResources");

	hResult = DirectX11Draw::pDevice_->QueryInterface(_uuidof(IDXGIDevice1), (void**)&(DirectX11Draw::pDXGIDevice_));

	massert(SUCCEEDED(hResult)
		&& "QueryInterfaceに失敗 @DirectX11Manager::InitializeCommonResources");

	hResult = DirectX11Draw::pDXGIDevice_->GetAdapter(&(DirectX11Draw::pDXGIAdapter_));
	massert(SUCCEEDED(hResult)
		&& "GetAdapterに失敗 @DirectX11Manager::InitializeCommonResources");

	hResult = DirectX11Draw::pDXGIAdapter_->GetParent(__uuidof(IDXGIFactory2), (void**)&(DirectX11Draw::pDXGIFactory_));
	massert(SUCCEEDED(hResult)
		&& "GetParentに失敗 @DirectX11Manager::InitializeCommonResources");

	InitializeShaderBundle();  // シェーダバンドルの初期化

	const D3D11_SAMPLER_DESC SAMPLER_DESC
	{
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
	};

	hResult = DirectX11Draw::pDevice_->CreateSamplerState(&SAMPLER_DESC, DirectX11Draw::pDefaultSamplerState_.GetAddressOf());
	massert(SUCCEEDED(hResult)
		&& "デフォルトのサンプラ作成に失敗 @DirectX11Manager::InitializeCommonResources");

	// 深度ステンシルの
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

	massert(SUCCEEDED(hResult)  // 深度ステンシルステートの作成に成功
		&& "深度ステンシルステートの作成に失敗");

#pragma region ブレンドステート作成
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

	massert(SUCCEEDED(hResult)  // ブレンドステート作成に成功
		&& "ブレンドステート作成に失敗");
#pragma endregion
	DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void mtgb::DirectX11Manager::CreateDXGISurface(IDXGISwapChain1* pSwapChain1, IDXGISurface** ppDXGISurface)
{
	HRESULT hResult{};
	
	//バックバッファ受け取る
	ID3D11Texture2D* pBackBuffer = nullptr;
	hResult = pSwapChain1->GetBuffer(0, _uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	massert(SUCCEEDED(hResult)
		&& "GetBufferに失敗 @DirectX11Manager::CreateDXGISurface");

	//バックバッファからIDXGISurfaceインターフェースを取り出す
	hResult = pBackBuffer->QueryInterface(IID_PPV_ARGS(ppDXGISurface));

	SAFE_RELEASE(pBackBuffer);

	massert(SUCCEEDED(hResult)
		&& "QueryInterfaceに失敗 @DirectX11Manager::CreateDXGISurface");
}

void mtgb::DirectX11Manager::CreateOutput(int index, IDXGIOutput** ppOutput)
{
	HRESULT hResult{};
	
	hResult = DirectX11Draw::pDXGIAdapter_->EnumOutputs(index, ppOutput);
	massert(SUCCEEDED(hResult)
		&& "EnumOutputsに失敗 @DirectX11Manager::CreateOutput");
}

void mtgb::DirectX11Manager::CreateSwapChain(HWND hWnd, IDXGIOutput* pOutput, IDXGISwapChain1** ppSwapChain1)
{
	HRESULT hResult{};
	
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	
	DXGI_SWAP_CHAIN_DESC1 desc
	{
		.Width = static_cast<UINT>(SCREEN_SIZE.x),//解像度(ピクセル数)。0ならウィンドウのサイズに合わせる
		.Height = static_cast<UINT>(SCREEN_SIZE.y),//解像度(ピクセル数)。0ならウィンドウのサイズに合わせる
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,  // 使える色数
		.Stereo = FALSE,//ステレオ(3D立体視)表示を有効にするか
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,  // 裏画面の枚数
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags = 0
	};

	hResult = DirectX11Draw::pDXGIFactory_->CreateSwapChainForHwnd(
		DirectX11Draw::pDevice_.Get(),
		hWnd,
		&desc,
		nullptr,//フルスクリーンの設定
		pOutput,//出力
		ppSwapChain1
	);
	massert(SUCCEEDED(hResult)
		&& "CreateSwapChainForHwndに失敗 @DirectX11Manager::CreateSwapChain");
}

void mtgb::DirectX11Manager::CreateRenderTargetView(IDXGISwapChain1* pSwapChain1, ID3D11RenderTargetView** ppRenderTargetView)
{
	HRESULT hResult{};
	
	ID3D11Texture2D* pBackBuffer{ nullptr };
	hResult = pSwapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	massert(SUCCEEDED(hResult) 
		&& "GetBufferに失敗 @DirectX11Manager::CreateRenderTargetView");

	hResult = DirectX11Draw::pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, ppRenderTargetView);
	massert(SUCCEEDED(hResult)
		&& "CreateRenderTargetViewに失敗 @DirectX11Manager::CreateRenderTargetView");

	pBackBuffer->Release();  // バックバッファは使わないため解放する
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
	
	// 深度バッファの設定
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

	massert(SUCCEEDED(hResult)  // 深度ステンシルバッファの作成に失敗
		&& "深度ステンシルバッファの作成に失敗");

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilView(
		*ppDepthStencil,
		nullptr,
		ppDepthStencilView);

	massert(SUCCEEDED(hResult)  // 深度ステンシルビュの作成に成功
		&& "深度ステンシルビューの作成に失敗");
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

	// 2D共通のインプットレイアウト
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

	// 3D共通のインプットレイアウト
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

	// 2D図形用シェーダの読み込み
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
		{
			.FillMode = D3D11_FILL_SOLID,  // 塗りつぶし: solid
			.CullMode = D3D11_CULL_BACK,  // カリング: 陰面消去
			.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
			.DepthBias = {},
			.DepthBiasClamp = {},
			.SlopeScaledDepthBias = {},
			.DepthClipEnable = true,//クリッピングを有効にする
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

	// 2Dスプライトシェーダの読み込み
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,  // 塗りつぶし: solid
				.CullMode = D3D11_CULL_NONE,  // カリング: 陰面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,//クリッピングを有効にする
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

	// FbxPartsシェーダの読み込み
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,  // 塗りつぶし: solid
				.CullMode = D3D11_CULL_BACK,  // カリング: 陰面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,//クリッピングを有効にする
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

	// Unlit3Dシェーダの読み込み
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,  // 塗りつぶし: solid
				.CullMode = D3D11_CULL_BACK,  // カリング: 陰面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,//クリッピングを有効にする
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

	// Debug3Dシェーダの読み込み
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_WIREFRAME,  // 枠だけ: wireframe
				.CullMode = D3D11_CULL_NONE,  // カリング: 隠面消去しない
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,//クリッピングを有効にする
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

#pragma region 頂点シェーダ
	// 項点シェーダのインタフェース
	ID3DBlob* pCompileVS{ nullptr };

	// 項点シェーダのコンパイル
	hResult = D3DCompileFromFile(
		_fileName.c_str(),  // ファイルパス
		nullptr,            // シェーダマクロの配列
		&hlslInclude,       // インクルードするやつ
		"VS",               // エントリポイントの関数名
		"vs_5_0",           // シェーダのバージョン (オプションで付けるやつ)
		0,                  // オプションフラグ1
		0,                  // オプションフラグ2
		&pCompileVS,        // コンパイル済みコードへのアクセスインタフェース
		nullptr);           // エラーメッセージ受信用 無し

	massert(SUCCEEDED(hResult)  // 頂点シェーダのコンパイルに成功
		&& "頂点シェーダのコンパイルに失敗 @DirectX11Manager::CompileShader");

	// 頂点シェーダを作成し、指定タイプのバンドルに格納する
	hResult = DirectX11Draw::pDevice_->CreateVertexShader(
		pCompileVS->GetBufferPointer(),  // コンパイルされたバッファのポインタ
		pCompileVS->GetBufferSize(),     // バッファのサイズ
		nullptr,                         // リンケージクラス: 無し
		&DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pVertexShader);

	massert(SUCCEEDED(hResult)  // 頂点シェーダの作成に成功
		&& "頂点シェーダの作成に失敗 @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region ピクセルシェーダ
	// ピクセルシェーダのインタフェース
	ID3DBlob* pCompilePS{ nullptr };

	// ピクセルシェーダのコンパイル
	hResult = D3DCompileFromFile(
		_fileName.c_str(),  // ファイルパス
		nullptr,            // シェーダマクロの配列
		&hlslInclude,       // インクルードするやつ
		"PS",               // エントリポイントの関数名
		"ps_5_0",           // シェーダのバージョン (オプションで付けるやつ)
		0,                  // オプションフラグ1
		0,                  // オプションフラグ2
		&pCompilePS,        // コンパイル済みコードへのアクセスインタフェース
		nullptr);           // エラーメッセージ受信用 無し

	massert(SUCCEEDED(hResult)  // ピクセルシェーダのコンパイルに成功
		&& "ピクセルシェーダのコンパイルに失敗 @DirectX11Manager::CompileShader");

	// ピクセルシェーダを作成し、指定タイプのバンドルに格納する
	hResult = DirectX11Draw::pDevice_->CreatePixelShader(
		pCompilePS->GetBufferPointer(),  // コンパイルされたバッファのポインタ
		pCompilePS->GetBufferSize(),     // バッファのサイズ
		nullptr,                         // リンケージクラス: 無し
		&DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pPixelShader);

	massert(SUCCEEDED(hResult)  // ピクセルシェーダの作成に成功
		&& "ピクセルシェーダの作成に失敗 @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region 頂点レイアウト
	// 頂点レイアウトを作成し、指定タイプのバンドルに格納する
	hResult = DirectX11Draw::pDevice_->CreateInputLayout(
		_pHLSLLayout,                    // 入力データ型配列
		_layoutLength,                   // 入力データ型配列の要素数
		pCompileVS->GetBufferPointer(),  // コンパイルされたバッファのポインタ
		pCompileVS->GetBufferSize(),     // バッファのサイズ
		&DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pVertexLayout);

	massert(SUCCEEDED(hResult)  // 頂点レイアウトの作成に成功
		&& "頂点レイアウトの作成に失敗 @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region ラスタライザ
	// ラスタライザを作成し、指定タイプのバンドルに格納する
	DirectX11Draw::pDevice_->CreateRasterizerState(
		_pRasterizerDesc,  // ラスタライザの設定
		&DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pRasterizerState);

	massert(SUCCEEDED(hResult)  // ラスタライザの作成に成功
		&& "ラスタライザの作成に失敗 @DirectX11Manager::CompileShader");
#pragma endregion


	// 解放していく
	pCompileVS->Release();
	pCompilePS->Release();
}
