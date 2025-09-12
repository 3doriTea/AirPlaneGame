#include "Texture2D.h"
#include <d3d11.h>
#include <wincodec.h>
#include "MTAssert.h"
#include "ReleaseUtility.h"
#include "DirectX11Draw.h"

mtgb::Texture2D::Texture2D() :
	pSamplerLinear_{ nullptr },
	pShaderResourceView_{ nullptr },
	size_{ Vector2Int::Zero() }
{
}

mtgb::Texture2D::~Texture2D()
{
	pSamplerLinear_.Reset();
	pShaderResourceView_.Reset();
}

void mtgb::Texture2D::Load(const std::wstring& _fileName)
{
	HRESULT hResult{};

	// TODO: COMを正しく理解する
	// MEMO: MTAに属するようにする
	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	massert(SUCCEEDED(hResult)  // CoInitializeExに成功している
		&& "CoInitializeExできなかった @Texture2D::Load");

	IWICImagingFactory* pFactory{ nullptr };
	IWICBitmapDecoder* pDecoder{ nullptr };
	IWICBitmapFrameDecode* pFrame{ nullptr };
	IWICFormatConverter* pFormatConverter{ nullptr };

	hResult = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void**>(&pFactory));

	massert(SUCCEEDED(hResult)  // IWICImagingFactoryの作成に成功
		&& "CoCreateInstanceに失敗 @Texture2D::Load");

	// CreateDecoderFromFilenameで対応している wchar へ変換
	//std::wstring fileNameWStr{ _fileName.begin(), _fileName.end() };

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/wincodec/nf-wincodec-iwicimagingfactory-createdecoderfromfilename
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/wincodec/ne-wincodec-wicdecodeoptions
	hResult = pFactory->CreateDecoderFromFilename(
		_fileName.c_str(),  // オブジェクトのファイル名
		nullptr,  // 優先したいベンダ: 指定無し
		GENERIC_READ,  // オブジェクトへ読み取りアクセス
		WICDecodeMetadataCacheOnDemand,  // 必要に応じてメタデータをキャッシュする
		&pDecoder);  // デコーダを渡す

	massert(SUCCEEDED(hResult)  // ファイルのデコードに成功
		&& "pFactory->CreateDecoderFromFilenameに失敗 @Texture2D::Load");

	// 0番フレームを取得
	// MEMO: GIFのような複数フレームある場合にindexたくさん指定できる
	hResult = pDecoder->GetFrame(0, &pFrame);

	massert(SUCCEEDED(hResult)  // 0番フレームを取得できている
		&& "0番フレームの取得に失敗 @Texture2D::Load");

	hResult = pFactory->CreateFormatConverter(&pFormatConverter);

	massert(SUCCEEDED(hResult)  // フォーマットコンバータの作成に成功
		&& "pFactory->CreateFormatConverterに失敗 @Texture2D::Load");

	// TODO: 変換のディザを理解する
	hResult = pFormatConverter->Initialize(
		pFrame,  // 変換したいやつ
		GUID_WICPixelFormat32bppRGBA,  // 変換先のピクセル形式
		WICBitmapDitherTypeNone,  // 使用ディザ(?)アルゴリズム
		nullptr,  // 使用パレット
		1.0,  // 使用アファ閾値
		WICBitmapPaletteTypeMedianCut);  // 中央値カットアルゴリズム(?)

	massert(SUCCEEDED(hResult)  // フォーマットコンバータの初期化に成功している
		&& "フォーマットコンバータの初期化に失敗 @Texture2D::Load");

	UINT imageWidth{};   // 画像の横幅
	UINT imageHeight{};  // 画像の縦幅
	hResult = pFormatConverter->GetSize(&imageWidth, &imageHeight);

	massert(SUCCEEDED(hResult)  // 画像サイズの取得に成功している
		&& "フォーマットコンバータから画像サイズの取得に失敗 @Texture2D::Load");

	// 画像のサイズをVector2Intに変換してメンバ変数に入れておく
	size_ = Vector2Int{ static_cast<int>(imageWidth), static_cast<int>(imageHeight) };

	// ピクセルデータを一時的に保存するバッファを用意
	std::vector<BYTE> pixelData(imageWidth * imageHeight * 4);
	UINT stride = imageWidth * 4;

	hResult = pFormatConverter->CopyPixels(
		nullptr,
		stride,
		static_cast<UINT>(pixelData.size()),
		pixelData.data());

	/*hResult = pFormatConverter->CopyPixels(
		nullptr,
		hMappedSubresource.RowPitch,
		imageHeight * hMappedSubresource.RowPitch,
		static_cast<BYTE*>(hMappedSubresource.pData));*/

	massert(SUCCEEDED(hResult)
		&& "テクスチャのピクセルコピーに失敗 @Texture2D::Load");

	// ミップマップレベルを計算
	int mipLevels = static_cast<int>(std::floor(std::log2((std::max)(imageWidth, imageHeight)))) + 1;

	ID3D11Texture2D* pTexture{ nullptr };

	const D3D11_TEXTURE2D_DESC TEXTURE2D_DESC  // テクスチャの設定
	{
		.Width = static_cast<UINT>(size_.x),
		.Height = static_cast<UINT>(size_.y),
		.MipLevels = static_cast<UINT>(mipLevels),
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0
		},
		.Usage = D3D11_USAGE_DEFAULT,
		//.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		//.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0,
		//.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS,
		//.MiscFlags = 0,
	};

	// 2Dテクスチャを作成する
	hResult = DirectX11Draw::pDevice_->CreateTexture2D(
		&TEXTURE2D_DESC,  // テクスチャの設定
		nullptr,         // サブリソースのポインタ
		&pTexture);      // 作成したテクスチャのポインタ渡し

	massert(SUCCEEDED(hResult)  //2Dテクスチャの作成に成功
		&& "2Dテクスチャの作成に失敗 @Texture2D::Load");

	D3D11_MAPPED_SUBRESOURCE hMappedSubresource{};

	//// テクスチャをマップ(登録)する
	//hResult = DirectX11Draw::pContext_->Map(
	//	pTexture,
	//	0U,
	//	D3D11_MAP_WRITE_DISCARD,
	//	0U,
	//	&hMappedSubresource);

	//massert(SUCCEEDED(hResult)  // テクスチャのマップに成功
	//	&& "テクスチャのマップに失敗 @Texture2D::Load");

	

	/*DirectX11Draw::pContext_->Unmap(pTexture, 0U);*/

	const D3D11_SHADER_RESOURCE_VIEW_DESC SHADER_RESOURCE_VIEW_DESC
	{
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D
		{
			.MostDetailedMip = 0U,
			.MipLevels = static_cast<UINT>(mipLevels),
		}
	};

	hResult = DirectX11Draw::pDevice_->CreateShaderResourceView(
		pTexture,
		&SHADER_RESOURCE_VIEW_DESC,
		pShaderResourceView_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // テクスチャ用シェーダリソースビューの作成に成功
		&& "テクスチャ用シェーダリソースビューの作成に失敗  @Texture2D::Load");

	// CPU側のデータをGPUへ書き込む
	DirectX11Draw::pContext_->UpdateSubresource(
		pTexture,           // リソース
		0,                  // サブリソースインデックス（最高解像度ミップ）
		nullptr,            // 更新範囲（全体）
		pixelData.data(),   // ピクセルデータ
		stride,             // 行ピッチ
		0);
	
	// ミップマップを生成
	DirectX11Draw::pContext_->GenerateMips(pShaderResourceView_.Get());

	// サンプラーステートの設定(ミップマップ対応に)
	const D3D11_SAMPLER_DESC SAMPLER_DESC  
	{
		// MEMO: サンプリングするときのフィルタ (かなり種類多い)
		//     : Unityのテクスチャ設定みたいな感じ
		//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_filter
		//.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		.MipLODBias = 0.0f,  // ミップマップのLODオフセット
		.MaxAnisotropy = 0U,  // 異方性フィルタを使った場合のクランプ値
		.ComparisonFunc = D3D11_COMPARISON_ALWAYS,  // TOOD: D3D11_COMPARISON_FUNCについて理解する
		.BorderColor = {},  // ボーダの色
		.MinLOD = {},  // LODミップマップの最小レベル
		.MaxLOD = D3D11_FLOAT32_MAX,  // LODミップマップの最大レベル
	};

	// サンプラーステートを作成
	hResult = DirectX11Draw::pDevice_->CreateSamplerState(&SAMPLER_DESC, pSamplerLinear_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)
		&& "サンプラーステートの作成に失敗 @Texture2D::Load");

	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pFormatConverter);
	SAFE_RELEASE(pFrame);
	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pFactory);

	CoUninitialize();
}
