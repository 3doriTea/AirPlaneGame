#include "DirectWrite.h"
#include "DirectX11Draw.h"
#include "MTAssert.h"
#include "Direct2D/Direct2D.h"
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <d2d1.h>
#include <dwrite.h> 
#include <dwrite_1.h>
#include <dxgidebug.h>

#include <clocale>
#include <cstdlib>
#pragma comment(lib,"dwrite.lib")

namespace
{
	std::string sampleText = "Hello,World!";
	const int DEFAULT_FONT_SIZE = 72;
	const wchar_t* fontFamilyName = L"Noto Sans JP";
}

IDWriteFactory* mtgb::DirectWrite::pDWriteFactory_{ nullptr };
IDWriteTextFormat* mtgb::DirectWrite::pTextFormat_{ nullptr };
//IDWriteTextLayout* mtgb::DirectWrite::pTextLayout_{ nullptr };
IDWriteFontCollection* mtgb::DirectWrite::pFontCollection_{ nullptr };
IDWriteFontFamily* mtgb::DirectWrite::pFontFamily_{nullptr};
IDWriteFont* mtgb::DirectWrite::pDWriteFont_{ nullptr };
DWRITE_FONT_METRICS mtgb::DirectWrite::fontMetrics_{};
mtgb::PixelFontMetrics mtgb::DirectWrite::pixelFontMetrics_;
//int mtgb::DirectWrite::currentDefaultFontSize_{ DEFAULT_FONT_SIZE };
static std::wstring StrToWStr(const std::string& str);

mtgb::FontFormatData::~FontFormatData()
{
	SAFE_RELEASE(format);
}

mtgb::TextLayoutData::~TextLayoutData()
{
	SAFE_RELEASE(layout);
}

mtgb::DirectWrite::DirectWrite()
{
	
	//currentDefaultFontSize_ = DEFAULT_FONT_SIZE;
}

mtgb::DirectWrite::~DirectWrite()
{
}

void mtgb::DirectWrite::Initialize()
{
	HRESULT hResult = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		_uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory_)
	);
	massert(SUCCEEDED(hResult)
		&& "DWriteCreateFactoryに失敗 @DirectWrite::Initialize");

	
}

void mtgb::DirectWrite::CreateFontFormatData(const std::wstring& fileName, int fontSize, FontFormatData** ppFontFormatData)
{
	//フォントコレクション取得
	HRESULT hResult = pDWriteFactory_->GetSystemFontCollection(&pFontCollection_);

	UINT32 index;
	BOOL exists;
	hResult = pFontCollection_->FindFamilyName(fileName.c_str(), &index, &exists);

	massert(SUCCEEDED(hResult)
		&& "FindFamilyNameに失敗 @DirectWrite::FindFamilyName");

	//ファミリーからIDWriteFontFamilyを取得
	hResult = pFontCollection_->GetFontFamily(index, &pFontFamily_);

	massert(SUCCEEDED(hResult)
		&& "GetFontFamilyに失敗 @DirectWrite::GetFontFamily");

	//IDWriteFontを取得
	hResult = pFontFamily_->GetFirstMatchingFont(
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STRETCH_NORMAL,//フォントのストレッチ。拡大縮小の度合い
		DWRITE_FONT_STYLE_NORMAL,//フォントスタイル
		&pDWriteFont_
	);
	massert(SUCCEEDED(hResult)
		&& "GetFirstMatchingFontに失敗 @DirectWrite::Initialize");

	//フォントのメトリクス取得
	pDWriteFont_->GetMetrics(&fontMetrics_);

	// デフォルトフォントフォーマットを作成
	//FontFormatData* formatData = new FontFormatData();
	// 新しいフォーマットを作成
	IDWriteTextFormat* format = nullptr;
	PixelFontMetrics metrics;

	CreateTextFormat(fontSize, &format, metrics);

	delete *ppFontFormatData;
	//FontFormatData* data = new FontFormatData(fontSize, format, metrics);
	*ppFontFormatData = new FontFormatData(fontSize, format, metrics);

	//pTextFormat_ = data->format;
	//pixelFontMetrics_ = data->pixelFontMetrics;

	
}

void mtgb::DirectWrite::Update()
{
}



void mtgb::DirectWrite::CreateTextFormat(int size, IDWriteTextFormat** ppTextFormat, PixelFontMetrics& outMetrics)
{
	//テキストフォーマット作成
	HRESULT hResult = pDWriteFactory_->CreateTextFormat(
		fontFamilyName,//フォントファミリーの名前
		NULL,//オリジナルのフォントを使う場合はそれ用のオブジェクトを渡す。NULLなら標準搭載のフォントファミリー
		DWRITE_FONT_WEIGHT_REGULAR,//フォントの太さ
		DWRITE_FONT_STYLE_NORMAL,//フォントスタイル
		DWRITE_FONT_STRETCH_NORMAL,//フォントのストレッチ。拡大縮小の度合い
		static_cast<FLOAT>(size),//フォントのサイズ
		L"en-us",//ロケール。en-usはアメリカ英語
		ppTextFormat//作成されるIDWriteTextFormat
	);

	massert(SUCCEEDED(hResult)
		&& "CreateTextFormatに失敗 @DirectWrite::CreateTextFormat");

	//水平方向
	hResult = (*ppTextFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	massert(SUCCEEDED(hResult)
		&& "SetTextAlignmentに失敗 @DirectWrite::CreateTextFormat");

	//垂直方向
	hResult = (*ppTextFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	massert(SUCCEEDED(hResult)
		&& "SetParagraphAlignmentに失敗 @DirectWrite::CreateTextFormat");

	//デザイン単位からピクセル単位に変換
	outMetrics.ascentPx = static_cast<float>(fontMetrics_.ascent) * static_cast<float>(size) / static_cast<float>(fontMetrics_.designUnitsPerEm);
	outMetrics.descentPx = static_cast<float>(fontMetrics_.descent) * static_cast<float>(size) / static_cast<float>(fontMetrics_.designUnitsPerEm);
	outMetrics.lineGapPx = static_cast<float>(fontMetrics_.lineGap) * static_cast<float>(size) / static_cast<float>(fontMetrics_.designUnitsPerEm);
	//テキストの上端を指定座標にほぼぴったり揃えるためのオフセット
	outMetrics.textTopOffset = -outMetrics.ascentPx + (outMetrics.ascentPx - outMetrics.descentPx) - outMetrics.lineGapPx;
}



void mtgb::DirectWrite::CreateTextLayout(const std::wstring& str, int size, IDWriteTextFormat* format, IDWriteTextLayout** ppTextLayout)
{
	//テキストレイアウト作成
	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();

	static const float dip = 96.0f;
	FLOAT dpiX, dpiY;
	mtgb::Direct2D::pRenderTarget_->GetDpi(&dpiX, &dpiY);
	
	rtSize.width = rtSize.width / dpiX * dip;
	rtSize.height = rtSize.height / dpiY * dip;

	//IDWriteTextLayout* layout;
	HRESULT hResult = pDWriteFactory_->CreateTextLayout(str.data(), str.size(), format, rtSize.width, rtSize.height, ppTextLayout);

	massert(SUCCEEDED(hResult)
		&& "CreateTextLayoutに失敗 @DirectWrite::RegisterText");
}



void mtgb::DirectWrite::ChangeFormat(IDWriteTextFormat* format, mtgb::PixelFontMetrics& metrics)
{
	pTextFormat_ = format;
	pixelFontMetrics_ = metrics;
}



void mtgb::DirectWrite::Draw(IDWriteTextLayout* textLayout, float x, float y)
{
	D2D1_POINT_2F origin = { x, y  };

	mtgb::Direct2D::pRenderTarget_->BeginDraw();
	mtgb::Direct2D::pRenderTarget_->DrawTextLayout(origin, textLayout, mtgb::Direct2D::pD2DBrush_);
	mtgb::Direct2D::pRenderTarget_->EndDraw();
}

void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text, float x, float y)
{
	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();

	mtgb::Direct2D::pRenderTarget_->BeginDraw();

	mtgb::Direct2D::pRenderTarget_->DrawText(
		text.c_str(),
		text.length(),
		pTextFormat_,
		D2D1::RectF(x, y + pixelFontMetrics_.textTopOffset, rtSize.width, rtSize.height),
		mtgb::Direct2D::pD2DBrush_
	);

	mtgb::Direct2D::pRenderTarget_->EndDraw();
}



//void mtgb::DirectWrite::ImmediateDraw(const std::string& text, float x, float y, int size)
//{
//	std::wstring wText = StrToWStr(text);
//	
//	// 指定サイズのフォーマットを取得または作成
//	auto formatData = GetOrCreateTextFormat(size);
//	IDWriteTextFormat* textFormat = formatData.first;
//	PixelFontMetrics metrics = formatData.second;
//
//	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();
//
//	mtgb::Direct2D::pRenderTarget_->BeginDraw();
//
//	mtgb::Direct2D::pRenderTarget_->DrawText(
//		wText.c_str(),
//		wText.length(),
//		textFormat,
//		D2D1::RectF(x, y + metrics.textTopOffset, rtSize.width, rtSize.height),
//		mtgb::Direct2D::pD2DBrush_
//	);
//
//	mtgb::Direct2D::pRenderTarget_->EndDraw();
//}

void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text,IDWriteTextFormat* format, const PixelFontMetrics& pixelFontMetrics, int x, int y)
{
	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();

	mtgb::Direct2D::pRenderTarget_->BeginDraw();

	mtgb::Direct2D::pRenderTarget_->DrawText(
		text.c_str(),
		text.length(),
		format,
		D2D1::RectF(x, y + pixelFontMetrics.textTopOffset, rtSize.width, rtSize.height),
		mtgb::Direct2D::pD2DBrush_
	);

	mtgb::Direct2D::pRenderTarget_->EndDraw();
}

void mtgb::DirectWrite::Release()
{
	//SAFE_RELEASE(pTextLayout_);
	SAFE_RELEASE(pTextFormat_);
	SAFE_RELEASE(pDWriteFactory_);

	/*if (textLayoutDatas_ != nullptr)
	{
		for (auto entry : *textLayoutDatas_)
		{
			SAFE_DELETE(entry);
		}
		textLayoutDatas_->clear();
		SAFE_DELETE(textLayoutDatas_);
	}

	if (fontFormatDatas_ != nullptr)
	{
		for (auto entry : *fontFormatDatas_)
		{
			SAFE_DELETE(entry);
		}
		fontFormatDatas_->clear();
		SAFE_DELETE(fontFormatDatas_);
	}*/

	ID3D11Debug* pDebug = nullptr;
	
	if (SUCCEEDED(mtgb::DirectX11Draw::pDevice_->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(& pDebug))))
	{
		// D3D11_RLO_DETAILで詳細なレポート
		pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		pDebug->Release();
	}
}

static std::wstring StrToWStr(const std::string& str)
{
	//nullptrを指定すると現在のロケール名が取得できる
	std::string oldLocale = setlocale(LC_CTYPE, nullptr);

	setlocale(LC_CTYPE, "Japanese_Japan.932");

	size_t len;
	//まずマルチバイト文字列(str)をワイド文字列に変換したときの長さを取得
	mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);
	//ワイド文字列のバッファ用意
	std::wstring wText;
	wText.resize(len);

	//実際にワイド文字列に変換
	//_TRUNCATE定数で、バッファに収まらない文字は切り捨てるよう指定する
	mbstowcs_s(&len, wText.data(), len, str.c_str(), _TRUNCATE);

	setlocale(LC_CTYPE, oldLocale.c_str());
	return wText;
}
