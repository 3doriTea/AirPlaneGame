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
#include "WindowContextUtil.h"
#include "Direct2DResource.h"
#include <clocale>
#include <cstdlib>
#pragma comment(lib,"dwrite.lib")

namespace
{
	std::string sampleText = "Hello,World!";
	const int DEFAULT_FONT_SIZE = 72;
	const wchar_t* fontFamilyName = L"Noto Sans JP";
}

ComPtr<IDWriteFactory> mtgb::DirectWrite::pDWriteFactory_{ nullptr };
ComPtr<IDWriteTextFormat> mtgb::DirectWrite::pTextFormat_{ nullptr };
//IDWriteTextLayout* mtgb::DirectWrite::pTextLayout_{ nullptr };
ComPtr<IDWriteFontCollection> mtgb::DirectWrite::pFontCollection_{ nullptr };
ComPtr<IDWriteFontFamily> mtgb::DirectWrite::pFontFamily_{nullptr};
ComPtr<IDWriteFont> mtgb::DirectWrite::pDWriteFont_{ nullptr };
DWRITE_FONT_METRICS mtgb::DirectWrite::fontMetrics_{};
mtgb::PixelFontMetrics mtgb::DirectWrite::pixelFontMetrics_;
//int mtgb::DirectWrite::currentDefaultFontSize_{ DEFAULT_FONT_SIZE };
//static std::wstring StrToWStr(const std::string& str);



mtgb::FontFormatData::FontFormatData(int size, IDWriteTextFormat* fmt, const PixelFontMetrics& metrics)
	: fontSize(size)
	, pixelFontMetrics(metrics)
{
	format.Attach(fmt);
}

mtgb::FontFormatData::~FontFormatData()
{
	format.Reset();
}

mtgb::TextLayoutData::TextLayoutData(const std::wstring& _str, int _size, float _width, float _height, IDWriteTextLayout* _layout, int _handle)
	: str(_str), fontSize(_size), width(_width), height(_height), handle(_handle)
{
	layout.Attach(_layout);
}

mtgb::TextLayoutData::~TextLayoutData()
{
	layout.Reset();
}

mtgb::DirectWrite::DirectWrite()
{
}

mtgb::DirectWrite::~DirectWrite()
{
}

void mtgb::DirectWrite::Initialize()
{
	HRESULT hResult = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		_uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(pDWriteFactory_.GetAddressOf())
	);
	massert(SUCCEEDED(hResult)
		&& "DWriteCreateFactoryに失敗 @DirectWrite::Initialize");

	
}

void mtgb::DirectWrite::CreateFontFormatData(const std::wstring& fileName, int fontSize, FontFormatData** ppFontFormatData)
{
	IDWriteFontCollection* fontCollection = nullptr;
	//フォントコレクション取得
	HRESULT hResult = pDWriteFactory_->GetSystemFontCollection(&fontCollection);
	pFontCollection_.Attach(fontCollection);

	UINT32 index;
	BOOL exists;
	hResult = pFontCollection_->FindFamilyName(fileName.c_str(), &index, &exists);

	massert(SUCCEEDED(hResult)
		&& "FindFamilyNameに失敗 @DirectWrite::FindFamilyName");

	//ファミリーからIDWriteFontFamilyを取得
	IDWriteFontFamily* fontFamily = nullptr;
	hResult = pFontCollection_->GetFontFamily(index, &fontFamily);
	pFontFamily_.Attach(fontFamily);

	massert(SUCCEEDED(hResult)
		&& "GetFontFamilyに失敗 @DirectWrite::GetFontFamily");

	//IDWriteFontを取得
	IDWriteFont* font = nullptr;
	hResult = pFontFamily_->GetFirstMatchingFont(
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STRETCH_NORMAL,//フォントのストレッチ。拡大縮小の度合い
		DWRITE_FONT_STYLE_NORMAL,//フォントスタイル
		&font
	);
	pDWriteFont_.Attach(font);
	massert(SUCCEEDED(hResult)
		&& "GetFirstMatchingFontに失敗 @DirectWrite::Initialize");

	//フォントのメトリクス取得
	pDWriteFont_->GetMetrics(&fontMetrics_);

	// 新しいフォーマットを作成
	IDWriteTextFormat* format = nullptr;
	PixelFontMetrics metrics;

	CreateTextFormat(fontSize, &format, metrics);

	delete *ppFontFormatData;
	//FontFormatData* data = new FontFormatData(fontSize, format, metrics);
	*ppFontFormatData = new FontFormatData(fontSize, format, metrics);

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

void mtgb::DirectWrite::SetTextAlignment(TextAlignment alignment, ComPtr<IDWriteTextFormat>format)
{
	switch (alignment)
	{
	case TextAlignment::topLeft:
		format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case TextAlignment::center:
		format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case TextAlignment::bottomLeft:
		format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case TextAlignment::middleLeft:
		format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	}
}

void mtgb::DirectWrite::CreateTextLayout(const std::wstring& _str, int  _size, ComPtr<IDWriteTextFormat> _format, IDWriteTextLayout** _ppTextLayout)
{
	//テキストレイアウト作成
	D2D1_SIZE_F rtSize = Game::System<Direct2D>().pDefRenderTarget_->GetSize();
	CreateTextLayout(_str, rtSize.width, rtSize.height, _size, _format, _ppTextLayout);
}

void mtgb::DirectWrite::CreateTextLayout(const std::wstring& _str, float  _width, float _height, int  _size, ComPtr<IDWriteTextFormat> _format, IDWriteTextLayout** _ppTextLayout)
{
	static const float dip = 96.0f;
	FLOAT dpiX, dpiY;
	Game::System<Direct2D>().pDefRenderTarget_->GetDpi(&dpiX, &dpiY);

	_width = _width / dpiX * dip;
	_height = _height / dpiY * dip;

	HRESULT hResult = pDWriteFactory_->CreateTextLayout(_str.data(), static_cast<uint32_t>(_str.size()), _format.Get(), _width, _height, _ppTextLayout);

	massert(SUCCEEDED(hResult)
		&& "CreateTextLayoutに失敗 @DirectWrite::RegisterText");
}

void mtgb::DirectWrite::ChangeFormat(ComPtr<IDWriteTextFormat> format, mtgb::PixelFontMetrics& metrics)
{
	pTextFormat_ = format;
	pixelFontMetrics_ = metrics;
}

void mtgb::DirectWrite::Draw(ComPtr<IDWriteTextLayout> textLayout, float x, float y)
{
	D2D1_POINT_2F origin = { x, y };

	Game::System<Direct2D>().pDefRenderTarget_->BeginDraw();
	Game::System<Direct2D>().pDefRenderTarget_->DrawTextLayout(origin, textLayout.Get(), Game::System<Direct2D>().pDefD2DBrush_.Get());
	Game::System<Direct2D>().pDefRenderTarget_->EndDraw();
}

void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text, float x, float y)
{
	ImmediateDraw(text, pTextFormat_, pixelFontMetrics_, x, y);
}

void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text, ComPtr<IDWriteTextFormat> format, const PixelFontMetrics& pixelFontMetrics, float x, float y, float width, float height)
{
	Game::System<Direct2D>().pDefRenderTarget_->BeginDraw();

	Game::System<Direct2D>().pDefRenderTarget_->DrawText(
		text.c_str(),
		static_cast<uint32_t>(text.length()),
		format.Get(),
		D2D1::RectF(
			x,
			y + pixelFontMetrics.textTopOffset,
			x + width,
			y + pixelFontMetrics.textTopOffset + height),
		Game::System<Direct2D>().pDefD2DBrush_.Get()
	);

	Game::System<Direct2D>().pDefRenderTarget_->EndDraw();
}

void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text, ComPtr<IDWriteTextFormat> format, const PixelFontMetrics& pixelFontMetrics, float x, float y)
{
	D2D1_SIZE_F rtSize = Game::System<Direct2D>().pDefRenderTarget_->GetSize();

	ImmediateDraw(text, format, pixelFontMetrics, x, y, rtSize.width, rtSize.height);
}



void mtgb::DirectWrite::Release()
{
	pDWriteFont_.Reset();
	pFontFamily_.Reset();
	pFontCollection_.Reset();
	pTextFormat_.Reset();
	pDWriteFactory_.Reset();

	
}

//static std::wstring StrToWStr(const std::string& str)
//{
//	//nullptrを指定すると現在のロケール名が取得できる
//	std::string oldLocale = setlocale(LC_CTYPE, nullptr);
//
//	setlocale(LC_CTYPE, "Japanese_Japan.932");
//
//	size_t len;
//	//まずマルチバイト文字列(str)をワイド文字列に変換したときの長さを取得
//	mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);
//	//ワイド文字列のバッファ用意
//	std::wstring wText;
//	wText.resize(len);
//
//	//実際にワイド文字列に変換
//	//_TRUNCATE定数で、バッファに収まらない文字は切り捨てるよう指定する
//	mbstowcs_s(&len, wText.data(), len, str.c_str(), _TRUNCATE);
//
//	setlocale(LC_CTYPE, oldLocale.c_str());
//	return wText;
//}
