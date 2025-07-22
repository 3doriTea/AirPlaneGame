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
IDWriteTextLayout* mtgb::DirectWrite::pTextLayout_{ nullptr };
IDWriteFontCollection* mtgb::DirectWrite::pFontCollection_{ nullptr };
IDWriteFontFamily* mtgb::DirectWrite::pFontFamily_{nullptr};
IDWriteFont* mtgb::DirectWrite::pDWriteFont_{ nullptr };
DWRITE_FONT_METRICS mtgb::DirectWrite::fontMetrics_{};
mtgb::PixelFontMetrics mtgb::DirectWrite::pixelFontMetrics_;
int mtgb::DirectWrite::currentDefaultFontSize_{ DEFAULT_FONT_SIZE };
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
	textLayoutDatas_ = new TextLayoutDatas();
	fontFormatDatas_ = new FontFormatDatas();
	currentDefaultFontSize_ = DEFAULT_FONT_SIZE;
	nextHandle_ = 0;
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
	auto formatData = GetOrCreateTextFormat(currentDefaultFontSize_);
	pTextFormat_ = formatData.first;
	pixelFontMetrics_ = formatData.second;
}

void mtgb::DirectWrite::Update()
{
}

std::pair<IDWriteTextFormat*, mtgb::PixelFontMetrics> mtgb::DirectWrite::GetOrCreateTextFormat(int size)
{
	auto& size_index = fontFormatDatas_->get<font_size_order>();
	auto it = size_index.find(size);
	
	if (it != size_index.end())
	{
		// 既存のフォーマットを返す
		return std::make_pair((*it)->format, (*it)->pixelFontMetrics);
	}

	// 新しいフォーマットを作成
	IDWriteTextFormat* format = nullptr;
	PixelFontMetrics metrics;
	CreateTextFormat(size, &format, metrics);

	// キャッシュに追加
	FontFormatData* data = new FontFormatData(size, format, metrics);
	fontFormatDatas_->insert(data);

	return std::make_pair(format, metrics);
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
	outMetrics.ascentPx = fontMetrics_.ascent * size / fontMetrics_.designUnitsPerEm;
	outMetrics.descentPx = fontMetrics_.descent * size / fontMetrics_.designUnitsPerEm;
	outMetrics.lineGapPx = fontMetrics_.lineGap * size / fontMetrics_.designUnitsPerEm;
	//テキストの上端を指定座標にほぼぴったり揃えるためのオフセット
	outMetrics.textTopOffset = -outMetrics.ascentPx + (outMetrics.ascentPx - outMetrics.descentPx) - outMetrics.lineGapPx;
}

int mtgb::DirectWrite::RegisterText(const std::string& str) 
{
	return RegisterText(str, currentDefaultFontSize_);
}

int mtgb::DirectWrite::RegisterText(const std::string& str, int size)
{
	std::wstring wText = StrToWStr(str);

	// 文字列+サイズの複合キーで検索
	auto& layout_index = textLayoutDatas_->get<text_layout_order>();
	auto it = layout_index.find(std::make_tuple(wText, size));
	
	if (it != layout_index.end())
	{
		// 既に同一のテキスト+サイズがあるのでそのハンドルを返す
		return (*it)->handle;
	}

	// フォーマットを取得または作成
	auto formatData = GetOrCreateTextFormat(size);
	IDWriteTextFormat* textFormat = formatData.first;

	//テキストレイアウト作成
	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();

	static const float dip = 96.0f;
	FLOAT dpiX, dpiY;
	mtgb::Direct2D::pRenderTarget_->GetDpi(&dpiX, &dpiY);
	
	rtSize.width = rtSize.width / dpiX * dip;
	rtSize.height = rtSize.height / dpiY * dip;

	IDWriteTextLayout* layout;
	HRESULT hResult = pDWriteFactory_->CreateTextLayout(wText.data(), wText.size(), textFormat, rtSize.width, rtSize.height, &layout);

	massert(SUCCEEDED(hResult)
		&& "CreateTextLayoutに失敗 @DirectWrite::RegisterText");

	int handle = nextHandle_++;
	TextLayoutData* entry = new TextLayoutData(wText, size, layout, handle);
	textLayoutDatas_->insert(entry);

	return handle;
}

void mtgb::DirectWrite::ChangeFontSize(int size)
{
	currentDefaultFontSize_ = size;
	// デフォルトフォーマットを更新
	auto formatData = GetOrCreateTextFormat(size);
	pTextFormat_ = formatData.first;
	pixelFontMetrics_ = formatData.second;
}

int mtgb::DirectWrite::Load(const std::string& str)
{
	DirectWrite& instance{ Game::System<DirectWrite>() };
	return instance.RegisterText(str, currentDefaultFontSize_);
}

int mtgb::DirectWrite::Load(const std::string& str, int size)
{
	DirectWrite& instance{ Game::System<DirectWrite>() };
	return instance.RegisterText(str, size);
}

void mtgb::DirectWrite::Draw(int handle, float x, float y)
{
	auto& handle_index = textLayoutDatas_->get<handle_order>();
	auto it = handle_index.find(handle);

	if (it == handle_index.end()) return;

	const auto& entry = *it;

	// そのテキストレイアウトに対応するフォントサイズのメトリクスを取得
	auto formatData = GetOrCreateTextFormat(entry->fontSize);
	PixelFontMetrics metrics = formatData.second;

	D2D1_POINT_2F origin = { x, y + metrics.textTopOffset };
	
	mtgb::Direct2D::pRenderTarget_->BeginDraw();
	mtgb::Direct2D::pRenderTarget_->DrawTextLayout(origin, entry->layout, mtgb::Direct2D::pD2DBrush_);
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

void mtgb::DirectWrite::ImmediateDraw(const std::string& text, float x, float y)
{
	std::wstring wText = StrToWStr(text);
	ImmediateDraw(wText, x, y);
}

void mtgb::DirectWrite::ImmediateDraw(const std::string& text, float x, float y, int size)
{
	std::wstring wText = StrToWStr(text);
	
	// 指定サイズのフォーマットを取得または作成
	auto formatData = GetOrCreateTextFormat(size);
	IDWriteTextFormat* textFormat = formatData.first;
	PixelFontMetrics metrics = formatData.second;

	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();

	mtgb::Direct2D::pRenderTarget_->BeginDraw();

	mtgb::Direct2D::pRenderTarget_->DrawText(
		wText.c_str(),
		wText.length(),
		textFormat,
		D2D1::RectF(x, y + metrics.textTopOffset, rtSize.width, rtSize.height),
		mtgb::Direct2D::pD2DBrush_
	);

	mtgb::Direct2D::pRenderTarget_->EndDraw();
}

void mtgb::DirectWrite::Release()
{
	SAFE_RELEASE(pTextLayout_);
	SAFE_RELEASE(pTextFormat_);
	SAFE_RELEASE(pDWriteFactory_);

	if (textLayoutDatas_ != nullptr)
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
	}

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
