#include "Text.h"
#include "DirectX11Draw.h"
#include "DirectWrite.h"
#include "MTStringUtility.h"
#include "DirectX11Draw.h"
int mtgb::Text::currentDefaultFontSize_{ 72 };
int mtgb::Text::nextHandle_{ 0 };
const wchar_t* mtgb::Text::DEFAULT_FONT_FAMILY_NAME{ L"Noto Sans JP" };
TextLayoutDatas* mtgb::Text::textLayoutDatas_{ nullptr };
FontFormatDatas* mtgb::Text::fontFormatDatas_{ nullptr };

namespace
{
	DirectWrite instance;
}
mtgb::Text::Text()
{
	textLayoutDatas_ = new TextLayoutDatas();
	fontFormatDatas_ = new FontFormatDatas();
}

mtgb::Text::~Text()
{
}

void mtgb::Text::Initialize()
{
	instance= Game::System<DirectWrite>();
	
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
	return GetOrCreateTextLayout(ToWString(str), size);
}

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

	instance.Draw(entry->layout, x, y + metrics.textTopOffset);
}



void mtgb::Text::ImmediateDraw(const std::wstring& text, float x, float y, int size)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	// 指定サイズのフォーマットを取得または作成
	auto formatData = GetOrCreateTextFormat(size);	
	
	instance.ImmediateDraw(text,formatData.first,formatData.second, static_cast<int>(x), static_cast<int>(y));
}

void mtgb::Text::ImmediateDraw(const std::string& text, float x, float y, int size)
{
	ImmediateDraw(ToWString(text), x, y, size);
}



int mtgb::Text::GetOrCreateTextLayout(const std::wstring& text, int size)
{
	// 文字列+サイズの複合キーで検索
	auto& layout_index = textLayoutDatas_->get<text_layout_order>();
	auto it = layout_index.find(std::make_tuple(text, size));

	if (it != layout_index.end())
	{
		// 既に同一のテキスト+サイズがあるのでそのハンドルを返す
		return (*it)->handle;
	}

	auto formatData = GetOrCreateTextFormat(size);

	IDWriteTextLayout* pTextLayout = nullptr;
	instance.CreateTextLayout(text, size, formatData.first, &pTextLayout);

	int handle = ++nextHandle_;
	TextLayoutData* layoutData = new TextLayoutData{ text,size,pTextLayout,handle};
	textLayoutDatas_->insert(layoutData);
	return handle;
}

std::pair<IDWriteTextFormat*, mtgb::PixelFontMetrics> mtgb::Text::GetOrCreateTextFormat(int size)
{
	auto& size_index = fontFormatDatas_->get<font_size_order>();
	auto it = size_index.find(size);

	if (it != size_index.end())
	{
		// 既存のフォーマットを返す
		return std::make_pair((*it)->format, (*it)->pixelFontMetrics);
	}

	FontFormatData* fontFormatData = nullptr;
	instance.CreateFontFormatData(DEFAULT_FONT_FAMILY_NAME, currentDefaultFontSize_, &fontFormatData);

	instance.ChangeFormat(fontFormatData->format, fontFormatData->pixelFontMetrics);
	fontFormatDatas_->insert(fontFormatData);

	return std::make_pair(fontFormatData->format, fontFormatData->pixelFontMetrics);
}

void mtgb::Text::ChangeFontSize(int size)
{
	currentDefaultFontSize_ = size;
	auto fontFormatData = GetOrCreateTextFormat(size);
	instance.ChangeFormat(fontFormatData.first,fontFormatData.second);
}