#include "DirectX11Manager.h"
#include "Game.h"
#include "MTAssert.h"
#include "IncludingWindows.h"
#include <d3d11.h>

#include <dxgi.h>
#include <DirectXMath.h>
#include "ImGuiRenderer.h"
#include "MainWindow.h"
#include "Screen.h"
#include "Vector3.h"
#include <d3dcompiler.h>
#include "HLSLInclude.h"
#include "WindowContext.h"
#include "ReleaseUtility.h"
#include "Direct2D/Direct2D.h"
#include "MTImGui.h"
#include "DirectX11Draw.h"

mtgb::DirectX11Manager::DirectX11Manager()
{
}

mtgb::DirectX11Manager::~DirectX11Manager()
{
}

void mtgb::DirectX11Manager::Initialize()
{

	InitializeCommonResources();
}

void mtgb::DirectX11Manager::Update()
{
	//MTImGui::Instance().DirectShow([this]() {
	//		for (auto& desc : adaptersDesc_)
	//		{
	//			ImGui::PushID(&desc);
	//			TypeRegistry::Instance().CallFunc(&desc, "AdapterDesc");
	//			ImGui::PopID();
	//			ImGui::Separator();
	//		}

	//		// モニター(DXGIOutput)の情報
	//		for (auto& monitorInfo : DirectX11Draw::monitorInfos_)
	//		{
	//			ImGui::PushID(&monitorInfo);
	//			ImGui::LabelText("adapterIndex", "%d", monitorInfo.adapterIndex);
	//			ImGui::LabelText("outputIndex","%d", monitorInfo.outputIndex);
	//			TypeRegistry::Instance().CallFunc(&monitorInfo.desc, "OutputDesc");
	//			ImGui::PopID();
	//			ImGui::Separator();
	//		}
	//	}, "Adapter,OutputDesc", ShowType::Settings);
}

void mtgb::DirectX11Manager::InitializeCommonResources()
{
	HRESULT hResult{};

	STARTUPINFO startupInfo{};
	GetStartupInfo(&startupInfo);
	int nCmdShow = startupInfo.wShowWindow;
	
	D3D_FEATURE_LEVEL level{};

	massert(SUCCEEDED(hResult)
		&& "QueryInterfaceに失敗 @DirectX11Manager::InitializeCommonResources");

	hResult = CreateDXGIFactory1(_uuidof(IDXGIFactory2), reinterpret_cast<void**>(DirectX11Draw::pDXGIFactory_.ReleaseAndGetAddressOf()));
	massert(SUCCEEDED(hResult)
		&& "CreateDXGIFactory1に失敗 @DirectX11Manager::InitializeCommonResources");

	UINT i = 0;
	ComPtr<IDXGIAdapter1> pAdapter = nullptr;
	while (DirectX11Draw::pDXGIFactory_->EnumAdapters1(i, pAdapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND)
	{
		DirectX11Draw::pDXGIAdapters_.push_back(pAdapter);
		++i;

		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1(&desc);
		adaptersDesc_.push_back(desc);
	}
	
	EnumAvailableMonitors(); // モニターの列挙

	hResult = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		DirectX11Draw::pDevice_.ReleaseAndGetAddressOf(),
		&level,
		DirectX11Draw::pContext_.ReleaseAndGetAddressOf()
	);
	massert(SUCCEEDED(hResult)
	 && "D3D11CreateDeviceに失敗 @DirectX11Manager::InitializeCommonResources");

	hResult = DirectX11Draw::pDevice_->QueryInterface(_uuidof(IDXGIDevice1), (void**)DirectX11Draw::pDXGIDevice_.ReleaseAndGetAddressOf());

	InitializeShaderBundle();  // シェーダバンドルの初期化

	const D3D11_SAMPLER_DESC SAMPLER_DESC
	{
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
	};

	hResult = DirectX11Draw::pDevice_->CreateSamplerState(&SAMPLER_DESC, DirectX11Draw::pDefaultSamplerState_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult)
		&& "デフォルトのサンプラ作成に失敗 @DirectX11Manager::InitializeCommonResources");
#pragma region 深度ステンシルステート作成

	// BlendMode::Defaultの作成
	D3D11_DEPTH_STENCIL_DESC DEPTH_STENCIL_DESC
	{
		.DepthEnable = TRUE,	//深度テストを行うかどうか
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		.DepthFunc = D3D11_COMPARISON_LESS_EQUAL, // 深度の比較方法 : LESS_EQUALは深度が元データ以下の場合に成功
		.StencilEnable = TRUE,  //ステンシルテストを行うかどうか
		.StencilReadMask = {},
		.StencilWriteMask = {},
		.FrontFace // カメラを向いているピクセルの深度、ステンシルテストの結果に対する操作を指定
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP, // ステンシルテスト失敗時
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP, // ステンシルテスト成功、深度テスト失敗時
			.StencilPassOp = D3D11_STENCIL_OP_KEEP, // 深度、ステンシルの両方のテストに成功時
			.StencilFunc = D3D11_COMPARISON_ALWAYS, // ステンシルデータと既存のステンシルデータを比較する関数(公式のをコピペ)
		},
		.BackFace // カメラを向いていないピクセルの深度、ステンシルテストの結果に対する操作を指定
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS,
		}
	};

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilState(
		&DEPTH_STENCIL_DESC,
		DirectX11Draw::pDepthStencilState_[static_cast<size_t>(BlendMode::Default)].ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // 深度ステンシルステートの作成に成功
		&& "BlendMode::Defaultの深度ステンシルステートの作成に失敗 @DirectX11Manager::InitializeCommonResources");

	// BlendMode::Spriteの作成
	DEPTH_STENCIL_DESC =
	{
		.DepthEnable = FALSE,	//深度テストを行うかどうか
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO,
		.DepthFunc = D3D11_COMPARISON_LESS_EQUAL,
		.StencilEnable = FALSE,  //ステンシルテストを行うかどうか
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
		DirectX11Draw::pDepthStencilState_[static_cast<size_t>(BlendMode::Sprite)].ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // 深度ステンシルステートの作成に成功
		&& "BlendMode::Spriteの深度ステンシルステートの作成に失敗 @DirectX11Manager::InitializeCommonResources");
#pragma endregion
#pragma region ブレンドステート作成

	// BlendMode::Defaultの作成
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
		DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::Default)].ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // ブレンドステート作成に成功
		&& "BlendMode::Defaultのブレンドステート作成に失敗 @DirectX11Manager::InitializeCommonResources");

	// BlendMode::Spriteの作成
	// 設定はBlendMode::Defaultと同じ
	hResult = DirectX11Draw::pDevice_->CreateBlendState(
		&BLEND_DESC,
		DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::Sprite)].ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // ブレンドステート作成に成功
		&& "ブレンドステート作成に失敗 @DirectX11Manager::InitializeCommonResources");
	
#pragma endregion
	DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void mtgb::DirectX11Manager::CreateDXGISurface(IDXGISwapChain1* pSwapChain1, IDXGISurface** ppDXGISurface)
{
	HRESULT hResult{};
	
	//バックバッファ受け取る
	ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
	hResult = pSwapChain1->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	massert(SUCCEEDED(hResult)
		&& "GetBufferに失敗 @DirectX11Manager::CreateDXGISurface");

	//バックバッファからIDXGISurfaceインターフェースを取り出す
	hResult = pBackBuffer->QueryInterface(IID_PPV_ARGS(ppDXGISurface));

	pBackBuffer.Reset();

	massert(SUCCEEDED(hResult)
		&& "QueryInterfaceに失敗 @DirectX11Manager::CreateDXGISurface");
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
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	// 仮のフラグ、
	bool fullscreen = false;
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = 
	{
		.RefreshRate
			{
				.Numerator = 60,
				.Denominator = 1
			},
		.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
		.Windowed = FALSE, // フルスクリーン
	};

	if (fullscreen)
	{
		hResult = DirectX11Draw::pDXGIFactory_->CreateSwapChainForHwnd(
			DirectX11Draw::pDevice_.Get(),
			hWnd,
			&desc,
			&fullscreenDesc,//フルスクリーンの設定
			pOutput,//出力
			ppSwapChain1
		);
	}
	else
	{
		hResult = DirectX11Draw::pDXGIFactory_->CreateSwapChainForHwnd(
			DirectX11Draw::pDevice_.Get(),
			hWnd,
			&desc,
			nullptr,//初期状態をフルスクリーンにしたい場合のみDESCを渡して、そうでないならnullptrにしておいて必要に応じてSetFullscreenStateで切り替える
			pOutput,//出力
			ppSwapChain1
		);
	}
	massert(SUCCEEDED(hResult)
		&& "CreateSwapChainForHwndに失敗 @DirectX11Manager::CreateSwapChain");
}

void mtgb::DirectX11Manager::CreateRenderTargetView(IDXGISwapChain1* pSwapChain1, ID3D11RenderTargetView** ppRenderTargetView)
{
	HRESULT hResult{};
	
	ComPtr<ID3D11Texture2D> pBackBuffer{ nullptr };
	hResult = pSwapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	massert(SUCCEEDED(hResult) 
		&& "GetBufferに失敗 @DirectX11Manager::CreateRenderTargetView");

	hResult = DirectX11Draw::pDevice_->CreateRenderTargetView(pBackBuffer.Get(), nullptr, ppRenderTargetView);
	massert(SUCCEEDED(hResult)
		&& "CreateRenderTargetViewに失敗 @DirectX11Manager::CreateRenderTargetView");

	pBackBuffer.Reset();
}

void mtgb::DirectX11Manager::CreateViewport(const Vector2Int& size, D3D11_VIEWPORT& viewport)
{
	viewport =
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = static_cast<float>(size.x),
		.Height = static_cast<float>(size.y),
		.MinDepth = 0,
		.MaxDepth = 1,
	};
}

void mtgb::DirectX11Manager::CreateDepthStencilAndDepthStencilView(const Vector2Int bufSize, ID3D11Texture2D** ppDepthStencil, ID3D11DepthStencilView** ppDepthStencilView)
{
	HRESULT hResult{};
	
	//const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	
	// 深度バッファの設定
	const D3D11_TEXTURE2D_DESC DEPTH_TEXTURE2D_DESC
	{
		.Width = static_cast<UINT>(bufSize.x),
		.Height = static_cast<UINT>(bufSize.y),
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

std::optional<mtgb::MonitorInfo> mtgb::DirectX11Manager::AssignAvailableMonitor(IDXGIOutput** ppOutput)
{
	// 初回の列挙
	/*if (DirectX11Draw::monitorInfos_.empty())
	{
		EnumAvailableMonitors();
	}*/

	// 未使用のモニターを探す
	for (auto& info : DirectX11Draw::monitorInfos_)
	{
		if (!info.isRequested)
		{
			HRESULT hResult = DirectX11Draw::pDXGIAdapters_[info.adapterIndex]->EnumOutputs(info.outputIndex, ppOutput);
			if (SUCCEEDED(hResult))
			{
				info.isRequested = true;
				return info;
			}
		}
	}

	// 全て使用済みの場合は、0番目のモニターを返す
	if (!DirectX11Draw::monitorInfos_.empty())
	{
		HRESULT hResult = DirectX11Draw::pDXGIAdapters_[0]->EnumOutputs(0, ppOutput);
		if (SUCCEEDED(hResult))
		{
			return DirectX11Draw::monitorInfos_[0];
		}
	}

	// モニターの割り当て失敗
	return std::nullopt;
}

int mtgb::DirectX11Manager::GetAvailableMonitorCount() const
{
	return static_cast<int>(DirectX11Draw::monitorInfos_.size());
}

void mtgb::DirectX11Manager::Release()
{
	DirectX11Draw::Release();
}

void mtgb::DirectX11Manager::ClearState()
{
	// パイプラインにバインドされた全てをリセット
	DirectX11Draw::pContext_->ClearState();
	// 描画コマンドを強制的にGPUに送り出す
	DirectX11Draw::pContext_->Flush();

	//ID3D11CommandList* pCmdList = nullptr;
	//// 描画コマンドが残っているなら破棄
	//DirectX11Draw::pContext_->FinishCommandList(FALSE, &pCmdList);
	//if (pCmdList)
	//{
	//	pCmdList->Release();
	//}
	Game::System<Direct2D>().Reset();
	DirectX11Draw::pDepthStencil_.Reset();
	DirectX11Draw::pDepthStencilView_.Reset();
	DirectX11Draw::pRenderTargetView_.Reset();
	//DirectX11Draw::pSwapChain1_.Reset();
}

void mtgb::DirectX11Manager::SetDefaultStates()
{
	// PrimitiveTopology を再設定
	DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// 必要に応じて他のデフォルト状態も再設定
	// 例：デフォルトサンプラーステート、ブレンドステートなど
	/*DirectX11Draw::pContext_->PSGetSamplers*/
}

void mtgb::DirectX11Manager::EnumAvailableMonitors()
{
	DirectX11Draw::monitorInfos_.clear();

	ComPtr<IDXGIAdapter1> pAdapter;
	for (UINT adapterIndex = 0; adapterIndex < DirectX11Draw::pDXGIAdapters_.size();adapterIndex++)
	{
		ComPtr<IDXGIOutput> pOutput;
		UINT outputIndex = 0;
		while(DirectX11Draw::pDXGIAdapters_[adapterIndex]->EnumOutputs(outputIndex, pOutput.GetAddressOf()) != DXGI_ERROR_NOT_FOUND)
		{
		
			MonitorInfo info{};
			info.adapterIndex = static_cast<int>(adapterIndex);
			info.outputIndex = outputIndex;
			info.isRequested = false;
			HRESULT hResult = pOutput->GetDesc(&info.desc);
			if (SUCCEEDED(hResult))
			{
				DirectX11Draw::monitorInfos_.push_back(info);
			}
			pOutput.Reset();
			outputIndex++;
		}
	}
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

	const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_DESC_SKINNED[]
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
		{
			.SemanticName = "BONE_INDEX",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_UINT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 3,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "BONE_WEIGHT",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 4,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
	};

	// 2D図形用シェーダの読み込み
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
		{
			.FillMode = D3D11_FILL_SOLID,   // 塗りつぶし: solid
			.CullMode = D3D11_CULL_BACK,    // カリング: 陰面消去
			.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
			.DepthBias = {},
			.DepthBiasClamp = {},
			.SlopeScaledDepthBias = {},
			.DepthClipEnable = true,        // クリッピングを有効にする
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
				.FillMode = D3D11_FILL_SOLID,   // 塗りつぶし: solid
				.CullMode = D3D11_CULL_NONE,    // カリング: 陰面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // クリッピングを有効にする
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
				.FillMode = D3D11_FILL_SOLID,   // 塗りつぶし: solid
				.CullMode = D3D11_CULL_BACK,    // カリング: 陰面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // クリッピングを有効にする
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
				.FillMode = D3D11_FILL_SOLID,   // 塗りつぶし: solid
				.CullMode = D3D11_CULL_BACK,    // カリング: 陰面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // クリッピングを有効にする
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
				.CullMode = D3D11_CULL_NONE,       // カリング: 隠面消去しない
				.FrontCounterClockwise = TRUE,     // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,           // クリッピングを有効にする
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

	// 地形シェーダの読み込み
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // 塗りつぶし
				.CullMode = D3D11_CULL_BACK,    // カリング: 隠面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // クリッピングを有効にする
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Ground.hlsl",
			ShaderType::Ground,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// Terrain
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // 塗りつぶし
				.CullMode = D3D11_CULL_BACK,    // カリング: 隠面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // クリッピングを有効にする
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Terrain.hlsl",
			ShaderType::Terrain,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// トレイルシェーダの読み込み
	{
		const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_DESC_TRAIL[]
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
				.SemanticName = "COLOR",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = vectorSize * 1,
				.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
		}; 
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // 塗りつぶし
				.CullMode = D3D11_CULL_NONE,    // カリング: 隠面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // クリッピングを有効にする
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Trail.hlsl",
			ShaderType::Trail,
			INPUT_ELEMENT_DESC_TRAIL,
			sizeof(INPUT_ELEMENT_DESC_TRAIL) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);

		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // 塗りつぶし: solid
				.CullMode = D3D11_CULL_BACK,    // カリング: 陰面消去
				.FrontCounterClockwise = TRUE,  // 三角形の正面向き = 時計回り
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // クリッピングを有効にする
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/FbxPartsSkin.hlsl",
			ShaderType::FbxPartsSkin,
			INPUT_ELEMENT_DESC_SKINNED,
			sizeof(INPUT_ELEMENT_DESC_SKINNED) / sizeof(D3D11_INPUT_ELEMENT_DESC),
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

	// 頂点シェーダのコンパイル
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
		DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pVertexShader.ReleaseAndGetAddressOf());

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
		DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pPixelShader.ReleaseAndGetAddressOf());

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
		DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pVertexLayout.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // 頂点レイアウトの作成に成功
		&& "頂点レイアウトの作成に失敗 @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region ラスタライザ
	// ラスタライザを作成し、指定タイプのバンドルに格納する
	DirectX11Draw::pDevice_->CreateRasterizerState(
		_pRasterizerDesc,  // ラスタライザの設定
		DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pRasterizerState.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // ラスタライザの作成に成功
		&& "ラスタライザの作成に失敗 @DirectX11Manager::CompileShader");
#pragma endregion


	// 解放していく
	SAFE_RELEASE(pCompileVS);
	SAFE_RELEASE(pCompilePS);
}
