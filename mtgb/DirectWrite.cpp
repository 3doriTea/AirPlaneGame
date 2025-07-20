
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
		&& "DWriteCreateFactory�Ɏ��s @DirectWrite::Initialize");


	const wchar_t* fontFamilyName = L"Noto Sans JP";

	//�e�L�X�g�t�H�[�}�b�g�쐬
	hResult =pDWriteFactory_->CreateTextFormat(
				fontFamilyName,//�t�H���g�t�@�~���[�̖��O
				NULL,//�I���W�i���̃t�H���g���g���ꍇ�͂���p�̃I�u�W�F�N�g��n���BNULL�Ȃ�W�����ڂ̃t�H���g�t�@�~���[
				DWRITE_FONT_WEIGHT_REGULAR,//�t�H���g�̑���
				DWRITE_FONT_STYLE_NORMAL,//�t�H���g�X�^�C��
				DWRITE_FONT_STRETCH_NORMAL,//�t�H���g�̃X�g���b�`�B�g��k���̓x����
		FONT_SIZE,//�t�H���g�̃T�C�Y
				L"en-us",//���P�[���Ben-us�̓A�����J�p��
				&pTextFormat_//�쐬�����IDWriteTextFormat
			);

	massert(SUCCEEDED(hResult)
		&& "CreateTextFormat�Ɏ��s @DirectWrite::Initialize");

	//��������
	hResult = pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	massert(SUCCEEDED(hResult)
		&& "SetTextAlignment�Ɏ��s @DirectWrite::Initialize");

	//��������
	hResult = pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	massert(SUCCEEDED(hResult)
		&& "SetParagraphAlignment�Ɏ��s @DirectWrite::Initialize");

	//�t�H���g�R���N�V�����擾
	hResult = pDWriteFactory_->GetSystemFontCollection(&pFontCollection_);

	UINT32 index;
	BOOL exists;
	hResult = pFontCollection_->FindFamilyName(fontFamilyName, &index, &exists);

	massert(SUCCEEDED(hResult)
		&& "FindFamilyName�Ɏ��s @DirectWrite::FindFamilyName");

	//�t�@�~���[����IDWriteFontFamily���擾
	hResult = pFontCollection_->GetFontFamily(index, &pFontFamily_);

	massert(SUCCEEDED(hResult)
		&& "GetFontFamily�Ɏ��s @DirectWrite::GetFontFamily");

	//IDWriteFont���擾
	hResult =	pFontFamily_->GetFirstMatchingFont(
					DWRITE_FONT_WEIGHT_REGULAR,
					DWRITE_FONT_STRETCH_NORMAL,//�t�H���g�̃X�g���b�`�B�g��k���̓x����
					DWRITE_FONT_STYLE_NORMAL,//�t�H���g�X�^�C��
					&pDWriteFont_
				);
	massert(SUCCEEDED(hResult)
		&& "GetFirstMatchingFont�Ɏ��s @DirectWrite::Initialize");

	//�t�H���g�̃��g���N�X�擾
	pDWriteFont_->GetMetrics(&fontMetrics_);

	//�f�U�C���P�ʂ���s�N�Z���P�ʂɕϊ�
	pixelFontMetrics_.ascentPx = fontMetrics_.ascent * FONT_SIZE / fontMetrics_.designUnitsPerEm;
	pixelFontMetrics_.descentPx = fontMetrics_.descent * FONT_SIZE / fontMetrics_.designUnitsPerEm;
	pixelFontMetrics_.lineGapPx = fontMetrics_.lineGap * FONT_SIZE / fontMetrics_.designUnitsPerEm;
	//�e�L�X�g�̏�[���w����W�ɂقڂ҂����葵���邽�߂̃I�t�Z�b�g
	pixelFontMetrics_.textTopOffset = - pixelFontMetrics_.ascentPx + (pixelFontMetrics_.ascentPx - pixelFontMetrics_.descentPx) - pixelFontMetrics_.lineGapPx;
}

void mtgb::DirectWrite::Update()
{
}

int mtgb::DirectWrite::RegisterText(const std::string& str) 
{
#if 0
	size_t len;
	//�܂��}���`�o�C�g������(str)�����C�h������ɕϊ������Ƃ��̒������擾
	mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);

	//���C�h������̃o�b�t�@�p��
	std::wstring wText;
	wText.resize(len);
	
	//���ۂɃ��C�h������ɕϊ�
	//_TRUNCATE�萔�ŁA�o�b�t�@�Ɏ��܂�Ȃ������͐؂�̂Ă�悤�w�肷��
	mbstowcs_s(&len, wText.data(), len, str.c_str(), _TRUNCATE);
#endif
	std::wstring wText = StrToWStr(str);

	//�������̃C���f�b�N�X���g��
	typedef TextLayoutSet::index<order>::type Order;
	auto& text_index = textLayouts_->get<order>();

	auto it = text_index.find(wText.data());
	if (it != text_index.end())
	{
		//���ɓ���̃e�L�X�g������̂ł��̃n���h����Ԃ�
		return (*it)->handle;
	}

	//�e�L�X�g���C�A�E�g�쐬
	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();

	static const float dip = 96.0f;
	FLOAT dpiX, dpiY;
	mtgb::Direct2D::pRenderTarget_->GetDpi(&dpiX, &dpiY);
	
	rtSize.width = rtSize.width / dpiX * dip;
	rtSize.height = rtSize.height / dpiY * dip;

	IDWriteTextLayout* layout;

	HRESULT hResult = pDWriteFactory_->CreateTextLayout(wText.data(), wText.size(), pTextFormat_, rtSize.width, rtSize.height, &layout);

	massert(SUCCEEDED(hResult)
		&& "CreateTextLayout�Ɏ��s @DirectWrite::RegisterText");

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
	//nullptr���w�肷��ƌ��݂̃��P�[�������擾�ł���
	std::string oldLocale = setlocale(LC_CTYPE, nullptr);

	setlocale(LC_CTYPE, "Japanese_Japan.932");

	size_t len;
	//�܂��}���`�o�C�g������(str)�����C�h������ɕϊ������Ƃ��̒������擾
	mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);
	//���C�h������̃o�b�t�@�p��
	std::wstring wText;
	wText.resize(len);

	//���ۂɃ��C�h������ɕϊ�
	//_TRUNCATE�萔�ŁA�o�b�t�@�Ɏ��܂�Ȃ������͐؂�̂Ă�悤�w�肷��
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
