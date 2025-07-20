
#include "DirectWrite.h"
#include "DirectX11Draw.h"
#include "MTAssert.h"
#include "Direct2D/Direct2D.h"


#include <d2d1.h>
#include <dwrite.h> 
#include <dwrite_1.h>

#include <clocale>
#include <cstdlib>
#pragma comment(lib,"dwrite.lib")

namespace
{
	std::string sampleText = "Hello,World!";
	const FLOAT FONT_SIZE = 72.0f;
}

IDWriteFactory* mtgb::DirectWrite::pDWriteFactory_{ nullptr };
IDWriteTextFormat* mtgb::DirectWrite::pTextFormat_{ nullptr };
IDWriteTextLayout* mtgb::DirectWrite::pTextLayout_{ nullptr };
IDWriteFontCollection* mtgb::DirectWrite::pFontCollection_{ nullptr };
IDWriteFontFamily* mtgb::DirectWrite::pFontFamily_{nullptr};
IDWriteFont* mtgb::DirectWrite::pDWriteFont_{ nullptr };
DWRITE_FONT_METRICS mtgb::DirectWrite::fontMetrics_{};
mtgb::PixelFontMetrics mtgb::DirectWrite::pixelFontMetrics_;

static std::wstring StrToWStr(const std::string& str);


mtgb::DirectWrite::DirectWrite()
{
	textLayouts_ = new TextLayoutSet();
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


	const wchar_t* fontFamilyName = L"Noto Sans JP";

	//テキストフォーマット作成
	hResult =pDWriteFactory_->CreateTextFormat(
				fontFamilyName,//フォントファミリーの名前
				NULL,//オリジナルのフォントを使う場合はそれ用のオブジェクトを渡す。NULLなら標準搭載のフォントファミリー
				DWRITE_FONT_WEIGHT_REGULAR,//フォントの太さ
				DWRITE_FONT_STYLE_NORMAL,//フォントスタイル
				DWRITE_FONT_STRETCH_NORMAL,//フォントのストレッチ。拡大縮小の度合い
		FONT_SIZE,//フォントのサイズ
				L"en-us",//ロケール。en-usはアメリカ英語
				&pTextFormat_//作成されるIDWriteTextFormat
			);

	massert(SUCCEEDED(hResult)
		&& "CreateTextFormatに失敗 @DirectWrite::Initialize");

	//水平方向
	hResult = pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	massert(SUCCEEDED(hResult)
		&& "SetTextAlignmentに失敗 @DirectWrite::Initialize");

	//垂直方向
	hResult = pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	massert(SUCCEEDED(hResult)
		&& "SetParagraphAlignmentに失敗 @DirectWrite::Initialize");

	//フォントコレクション取得
	hResult = pDWriteFactory_->GetSystemFontCollection(&pFontCollection_);

	UINT32 index;
	BOOL exists;
	hResult = pFontCollection_->FindFamilyName(fontFamilyName, &index, &exists);

	massert(SUCCEEDED(hResult)
		&& "FindFamilyNameに失敗 @DirectWrite::FindFamilyName");

	//ファミリーからIDWriteFontFamilyを取得
	hResult = pFontCollection_->GetFontFamily(index, &pFontFamily_);

	massert(SUCCEEDED(hResult)
		&& "GetFontFamilyに失敗 @DirectWrite::GetFontFamily");

	//IDWriteFontを取得
	hResult =	pFontFamily_->GetFirstMatchingFont(
					DWRITE_FONT_WEIGHT_REGULAR,
					DWRITE_FONT_STRETCH_NORMAL,//フォントのストレッチ。拡大縮小の度合い
					DWRITE_FONT_STYLE_NORMAL,//フォントスタイル
					&pDWriteFont_
				);
	massert(SUCCEEDED(hResult)
		&& "GetFirstMatchingFontに失敗 @DirectWrite::Initialize");

	//フォントのメトリクス取得
	pDWriteFont_->GetMetrics(&fontMetrics_);

	//デザイン単位からピクセル単位に変換
	pixelFontMetrics_.ascentPx = fontMetrics_.ascent * FONT_SIZE / fontMetrics_.designUnitsPerEm;
	pixelFontMetrics_.descentPx = fontMetrics_.descent * FONT_SIZE / fontMetrics_.designUnitsPerEm;
	pixelFontMetrics_.lineGapPx = fontMetrics_.lineGap * FONT_SIZE / fontMetrics_.designUnitsPerEm;
	//テキストの上端を指定座標にほぼぴったり揃えるためのオフセット
	pixelFontMetrics_.textTopOffset = - pixelFontMetrics_.ascentPx + (pixelFontMetrics_.ascentPx - pixelFontMetrics_.descentPx) - pixelFontMetrics_.lineGapPx;
}

void mtgb::DirectWrite::Update()
{
}

int mtgb::DirectWrite::RegisterText(const std::string& str) 
{
#if 0
	size_t len;
	//まずマルチバイト文字列(str)をワイド文字列に変換したときの長さを取得
	mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);

	//ワイド文字列のバッファ用意
	std::wstring wText;
	wText.resize(len);
	
	//実際にワイド文字列に変換
	//_TRUNCATE定数で、バッファに収まらない文字は切り捨てるよう指定する
	mbstowcs_s(&len, wText.data(), len, str.c_str(), _TRUNCATE);
#endif
	std::wstring wText = StrToWStr(str);

	//辞書順のインデックスを使う
	typedef TextLayoutSet::index<order>::type Order;
	auto& text_index = textLayouts_->get<order>();

	auto it = text_index.find(wText.data());
	if (it != text_index.end())
	{
		//既に同一のテキストがあるのでそのハンドルを返す
		return (*it)->handle;
	}

	//テキストレイアウト作成
	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();

	static const float dip = 96.0f;
	FLOAT dpiX, dpiY;
	mtgb::Direct2D::pRenderTarget_->GetDpi(&dpiX, &dpiY);
	
	rtSize.width = rtSize.width / dpiX * dip;
	rtSize.height = rtSize.height / dpiY * dip;

	IDWriteTextLayout* layout;

	HRESULT hResult = pDWriteFactory_->CreateTextLayout(wText.data(), wText.size(), pTextFormat_, rtSize.width, rtSize.height, &layout);

	massert(SUCCEEDED(hResult)
		&& "CreateTextLayoutに失敗 @DirectWrite::RegisterText");

	int handle = static_cast<int>(textLayouts_->size());
	TextLayoutEntry* entry = new TextLayoutEntry{ wText,layout,handle };
	textLayouts_->insert(entry);

	return handle;
}

int mtgb::DirectWrite::Load(const std::string& str)
{
	DirectWrite& instance{ Game::System<DirectWrite>() };

	return instance.RegisterText(str);
}
void mtgb::DirectWrite::Draw(int handle, float x, float y)
{
	auto& handle_index = textLayouts_->get<random>();

	if (handle >= handle_index.size()) return;

	const auto& entry = handle_index[handle];

	DWRITE_TEXT_METRICS metrics;
	entry->layout->GetMetrics(&metrics);
	FLOAT size = entry->layout->GetFontSize();
	D2D1_POINT_2F origin = { x,y + pixelFontMetrics_.textTopOffset };
	
	mtgb::Direct2D::pRenderTarget_->BeginDraw();

	mtgb::Direct2D::pRenderTarget_->DrawTextLayout(origin, entry->layout, mtgb::Direct2D::pD2DBrush_);

	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();

	//mtgb::Direct2D::pRenderTarget_->DrawText(text.c_str(), text.length(), pTextFormat_, D2D1::RectF(0,0,rtSize.width,rtSize.height), mtgb::Direct2D::pD2DBrush_);

	mtgb::Direct2D::pRenderTarget_->EndDraw();
}

void mtgb::DirectWrite::Release()
{
	SAFE_RELEASE(pTextLayout_);
	SAFE_RELEASE(pTextFormat_);
	SAFE_RELEASE(pDWriteFactory_);

	if (textLayouts_ != nullptr)
	{
		for (auto entry : *textLayouts_)
		{
			SAFE_DELETE(entry);
		}
		textLayouts_->clear();
		SAFE_DELETE(textLayouts_);
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

//mtgb::TextLayoutEntry::TextLayoutEntry()
//{
//	layout = nullptr;
//}

mtgb::TextLayoutEntry::~TextLayoutEntry()
{
	SAFE_RELEASE(layout);
}
