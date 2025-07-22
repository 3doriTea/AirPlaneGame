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
		&& "DWriteCreateFactory�Ɏ��s @DirectWrite::Initialize");

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
	hResult = pFontFamily_->GetFirstMatchingFont(
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STRETCH_NORMAL,//�t�H���g�̃X�g���b�`�B�g��k���̓x����
		DWRITE_FONT_STYLE_NORMAL,//�t�H���g�X�^�C��
		&pDWriteFont_
	);
	massert(SUCCEEDED(hResult)
		&& "GetFirstMatchingFont�Ɏ��s @DirectWrite::Initialize");

	//�t�H���g�̃��g���N�X�擾
	pDWriteFont_->GetMetrics(&fontMetrics_);

	// �f�t�H���g�t�H���g�t�H�[�}�b�g���쐬
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
		// �����̃t�H�[�}�b�g��Ԃ�
		return std::make_pair((*it)->format, (*it)->pixelFontMetrics);
	}

	// �V�����t�H�[�}�b�g���쐬
	IDWriteTextFormat* format = nullptr;
	PixelFontMetrics metrics;
	CreateTextFormat(size, &format, metrics);

	// �L���b�V���ɒǉ�
	FontFormatData* data = new FontFormatData(size, format, metrics);
	fontFormatDatas_->insert(data);

	return std::make_pair(format, metrics);
}

void mtgb::DirectWrite::CreateTextFormat(int size, IDWriteTextFormat** ppTextFormat, PixelFontMetrics& outMetrics)
{
	//�e�L�X�g�t�H�[�}�b�g�쐬
	HRESULT hResult = pDWriteFactory_->CreateTextFormat(
		fontFamilyName,//�t�H���g�t�@�~���[�̖��O
		NULL,//�I���W�i���̃t�H���g���g���ꍇ�͂���p�̃I�u�W�F�N�g��n���BNULL�Ȃ�W�����ڂ̃t�H���g�t�@�~���[
		DWRITE_FONT_WEIGHT_REGULAR,//�t�H���g�̑���
		DWRITE_FONT_STYLE_NORMAL,//�t�H���g�X�^�C��
		DWRITE_FONT_STRETCH_NORMAL,//�t�H���g�̃X�g���b�`�B�g��k���̓x����
		static_cast<FLOAT>(size),//�t�H���g�̃T�C�Y
		L"en-us",//���P�[���Ben-us�̓A�����J�p��
		ppTextFormat//�쐬�����IDWriteTextFormat
	);

	massert(SUCCEEDED(hResult)
		&& "CreateTextFormat�Ɏ��s @DirectWrite::CreateTextFormat");

	//��������
	hResult = (*ppTextFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	massert(SUCCEEDED(hResult)
		&& "SetTextAlignment�Ɏ��s @DirectWrite::CreateTextFormat");

	//��������
	hResult = (*ppTextFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	massert(SUCCEEDED(hResult)
		&& "SetParagraphAlignment�Ɏ��s @DirectWrite::CreateTextFormat");

	//�f�U�C���P�ʂ���s�N�Z���P�ʂɕϊ�
	outMetrics.ascentPx = fontMetrics_.ascent * size / fontMetrics_.designUnitsPerEm;
	outMetrics.descentPx = fontMetrics_.descent * size / fontMetrics_.designUnitsPerEm;
	outMetrics.lineGapPx = fontMetrics_.lineGap * size / fontMetrics_.designUnitsPerEm;
	//�e�L�X�g�̏�[���w����W�ɂقڂ҂����葵���邽�߂̃I�t�Z�b�g
	outMetrics.textTopOffset = -outMetrics.ascentPx + (outMetrics.ascentPx - outMetrics.descentPx) - outMetrics.lineGapPx;
}

int mtgb::DirectWrite::RegisterText(const std::string& str) 
{
	return RegisterText(str, currentDefaultFontSize_);
}

int mtgb::DirectWrite::RegisterText(const std::string& str, int size)
{
	std::wstring wText = StrToWStr(str);

	// ������+�T�C�Y�̕����L�[�Ō���
	auto& layout_index = textLayoutDatas_->get<text_layout_order>();
	auto it = layout_index.find(std::make_tuple(wText, size));
	
	if (it != layout_index.end())
	{
		// ���ɓ���̃e�L�X�g+�T�C�Y������̂ł��̃n���h����Ԃ�
		return (*it)->handle;
	}

	// �t�H�[�}�b�g���擾�܂��͍쐬
	auto formatData = GetOrCreateTextFormat(size);
	IDWriteTextFormat* textFormat = formatData.first;

	//�e�L�X�g���C�A�E�g�쐬
	D2D1_SIZE_F rtSize = mtgb::Direct2D::pRenderTarget_->GetSize();

	static const float dip = 96.0f;
	FLOAT dpiX, dpiY;
	mtgb::Direct2D::pRenderTarget_->GetDpi(&dpiX, &dpiY);
	
	rtSize.width = rtSize.width / dpiX * dip;
	rtSize.height = rtSize.height / dpiY * dip;

	IDWriteTextLayout* layout;
	HRESULT hResult = pDWriteFactory_->CreateTextLayout(wText.data(), wText.size(), textFormat, rtSize.width, rtSize.height, &layout);

	massert(SUCCEEDED(hResult)
		&& "CreateTextLayout�Ɏ��s @DirectWrite::RegisterText");

	int handle = nextHandle_++;
	TextLayoutData* entry = new TextLayoutData(wText, size, layout, handle);
	textLayoutDatas_->insert(entry);

	return handle;
}

void mtgb::DirectWrite::ChangeFontSize(int size)
{
	currentDefaultFontSize_ = size;
	// �f�t�H���g�t�H�[�}�b�g���X�V
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

	// ���̃e�L�X�g���C�A�E�g�ɑΉ�����t�H���g�T�C�Y�̃��g���N�X���擾
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
	
	// �w��T�C�Y�̃t�H�[�}�b�g���擾�܂��͍쐬
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
		// D3D11_RLO_DETAIL�ŏڍׂȃ��|�[�g
		pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		pDebug->Release();
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
