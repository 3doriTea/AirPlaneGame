#include "Text.h"
#include "DirectX11Draw.h"
#include "DirectWrite.h"
#include "MTStringUtility.h"
#include <dwrite.h> 
#include "Direct2D/Direct2D.h"
const wchar_t* mtgb::Text::DEFAULT_FONT_FAMILY_NAME{ L"Noto Sans JP" };


namespace
{
}
mtgb::Text::Text()
	:nextHandle_{0}
	,currentDefaultFontSize_{72}
{
	textLayoutDatas_ = new TextLayoutDatas();
	fontFormatDatas_ = new FontFormatDatas();
}

mtgb::Text::~Text()
{
}

void mtgb::Text::Initialize()
{
}

void mtgb::Text::Release()
{
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
}

void mtgb::Text::Update()
{
}

int mtgb::Text::Load(const std::string& str, int size)
{
	return Game::System<Text>().GetOrCreateTextLayoutHandle(MultiToWide(str), size);
}

int mtgb::Text::Load(const std::string& str, int fontSize, float layoutBoxWidth, float layoutBoxHeight)
{
	return Game::System<Text>().GetOrCreateTextLayoutHandle(MultiToWide(str), fontSize,layoutBoxWidth,layoutBoxHeight);
}

int mtgb::Text::Load(const std::string& str, int fontSize, Vector2F layoutBoxSize)
{
	return Game::System<Text>().GetOrCreateTextLayoutHandle(MultiToWide(str), fontSize, layoutBoxSize.x, layoutBoxSize.y);
}

#if false
void mtgb::Text::Draw(int handle, float x, float y)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	auto& handle_index = textLayoutDatas_->get<handle_order>();
	auto it = handle_index.find(handle);

	if (it == handle_index.end()) return;

	const auto& entry = *it;

	// そのテキストレイアウトに対応するフォントサイズのメトリクスを取得
	auto formatData = GetOrCreateTextFormat(entry->fontSize);
	PixelFontMetrics metrics = formatData.second;
	
	Game::System<DirectWrite>().SetTextAlignment(currentTextAlignment_, entry->layout);
	Game::System<DirectWrite>().Draw(entry->layout, x, y + metrics.textTopOffset);
}



void mtgb::Text::ImmediateDraw(const std::wstring& text, float x, float y, int size)
{
	D2D1_SIZE_F layoutBox = Game::System<Direct2D>().GetRenderTargetSize();
}

void mtgb::Text::ImmediateDraw(const std::string& text, float x, float y, int size)
{
	ImmediateDraw(ToWString(text), x, y, size);
}

void mtgb::Text::ImmediateDraw(const std::string& text, RectInt rect, int size)
{
}

void mtgb::Text::ImmediateDraw(const std::string& text, float x, float y)
{
	ImmediateDraw(text, x, y, currentDefaultFontSize_);
}

void mtgb::Text::ImmediateDraw(const std::wstring& text, float x, float y)
{
	ImmediateDraw(text, x, y, currentDefaultFontSize_);

}

void mtgb::Text::ImmediateDraw(const std::wstring& text, float x, float y, float width, float height, int size)
{
	// 指定サイズのフォーマットを取得または作成
	auto formatData = GetOrCreateTextFormat(size);

	Game::System<DirectWrite>().SetTextAlignment(currentTextAlignment_, formatData.first);
	Game::System<DirectWrite>().ImmediateDraw(text, formatData.first, formatData.second, x, y, width,height);
}

void mtgb::Text::ChangeFontSize(int size)
{
	currentDefaultFontSize_ = size;
	auto fontFormatData = GetOrCreateTextFormat(size);
	Game::System<DirectWrite>().ChangeFormat(fontFormatData.first, fontFormatData.second);
}

void mtgb::Text::ChangeTextAlignment(TextAlignment alignment)
{
	currentTextAlignment_ = alignment;
}

#endif

int mtgb::Text::GetOrCreateTextLayoutHandle(const std::wstring& text, int size)
{
	D2D1_SIZE_F layoutBoxSize = Game::System<Direct2D>().GetRenderTargetSize();
	return GetOrCreateTextLayoutHandle(text, size, layoutBoxSize.width, layoutBoxSize.height);
}

int mtgb::Text::GetOrCreateTextLayoutHandle(const std::wstring& text, int size, float width, float height)
{
	// text + size の範囲を取得
	auto& text_index = textLayoutDatas_->get<text_layout_order>();
	auto range = text_index.equal_range(std::make_tuple(text, size));
	for (auto it = range.first; it != range.second; ++it)
	{
		// width / height も一致するものを探す
		TextLayoutData* data = *it;
		if (data->width == width && data->height == height)
		{
			return data->handle; // 完全一致があれば再利用
		}
	}

	// 新規作成する
	FontFormatData* formatData = GetOrCreateTextFormat(size);

	IDWriteTextLayout* pTextLayout = nullptr;
	Game::System<DirectWrite>().CreateTextLayout(text, width, height, size, formatData->format, &pTextLayout);

	int handle = ++nextHandle_;
	TextLayoutData* layoutData = new TextLayoutData{ text,size, width,height,pTextLayout,handle };
	textLayoutDatas_->insert(layoutData);
	return handle;
}

TextLayoutData* mtgb::Text::GetTextLayoutData(int handle)
{
	auto& handle_index = textLayoutDatas_->get<handle_order>();
	auto it = handle_index.find(handle);

	if (it == handle_index.end()) return nullptr;

	return *it;
}

FontFormatData* mtgb::Text::GetOrCreateTextFormat(int size)
{
	auto& size_index = fontFormatDatas_->get<font_size_order>();
	auto it = size_index.find(size);

	if (it != size_index.end())
	{
		// 既存のフォーマットを返す
		return *it;
	}

	FontFormatData* fontFormatData = nullptr;
	Game::System<DirectWrite>().CreateFontFormatData(DEFAULT_FONT_FAMILY_NAME, size, &fontFormatData);

	Game::System<DirectWrite>().ChangeFormat(fontFormatData->format, fontFormatData->pixelFontMetrics);
	fontFormatDatas_->insert(fontFormatData);

	return fontFormatData;
}


