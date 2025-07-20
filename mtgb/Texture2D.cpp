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
	SAFE_RELEASE(pSamplerLinear_);
	SAFE_RELEASE(pShaderResourceView_);
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

	ID3D11Texture2D* pTexture{ nullptr };

	const D3D11_TEXTURE2D_DESC TEXTURE2D_DESC  // テクスチャの設定
	{
		.Width = static_cast<UINT>(size_.x),
		.Height = static_cast<UINT>(size_.y),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0
		},
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
	};

	// 2Dテクスチャを作成する
	hResult = DirectX11Draw::pDevice_->CreateTexture2D(
		&TEXTURE2D_DESC,  // テクスチャの設定
		nullptr,         // サブリソースのポインタ
		&pTexture);      // 作成したテクスチャのポインタ渡し

	massert(SUCCEEDED(hResult)  //2Dテクスチャの作成に成功
		&& "2Dテクスチャの作成に失敗 @Texture2D::Load");

	D3D11_MAPPED_SUBRESOURCE hMappedSubresource{};

	// テクスチャをマップ(登録)する
	hResult = DirectX11Draw::pContext_->Map(
		pTexture,
		0U,
		D3D11_MAP_WRITE_DISCARD,
		0U,
		&hMappedSubresource);

	massert(SUCCEEDED(hResult)  // テクスチャのマップに成功
		&& "テクスチャのマップに失敗 @Texture2D::Load");

	hResult = pFormatConverter->CopyPixels(
		nullptr,
		hMappedSubresource.RowPitch,
		imageHeight * hMappedSubresource.RowPitch,
		static_cast<BYTE*>(hMappedSubresource.pData));

	massert(SUCCEEDED(hResult)
		&& "テクスチャのピクセルコピーに失敗 @Texture2D::Load");

	DirectX11Draw::pContext_->Unmap(pTexture, 0U);

	const D3D11_SHADER_RESOURCE_VIEW_DESC SHADER_RESOURCE_VIEW_DESC
	{
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D
		{
			.MostDetailedMip = 0U,
			.MipLevels = 1U,
		}
	};

	hResult = DirectX11Draw::pDevice_->CreateShaderResourceView(
		pTexture,
		&SHADER_RESOURCE_VIEW_DESC,
		&pShaderResourceView_);

	massert(SUCCEEDED(hResult)  // テクスチャ用シェーダリソースビューの作成に成功
		&& "テクスチャ用シェーダリソースビューの作成に失敗  @Texture2D::Load");

	const D3D11_SAMPLER_DESC SAMPLER_DESC  // サンプラーステートの設定
	{
		// MEMO: サンプリングするときのフィルタ (かなり種類多い)
		//     : Unityのテクスチャ設定みたいな感じ
		//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_filter
		.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		//.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
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
	hResult = DirectX11Draw::pDevice_->CreateSamplerState(&SAMPLER_DESC, &pSamplerLinear_);

	massert(SUCCEEDED(hResult)
		&& "サンプラーステートの作成に失敗 @Texture2D::Load");

	pTexture->Release();
	pFormatConverter->Release();
	pFrame->Release();
	pDecoder->Release();
	pFactory->Release();

	CoUninitialize();
}
