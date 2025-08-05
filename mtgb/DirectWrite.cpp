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
		&& "DWriteCreateFactory�Ɏ��s @DirectWrite::Initialize");

	
}

void mtgb::DirectWrite::CreateFontFormatData(const std::wstring& fileName, int fontSize, FontFormatData** ppFontFormatData)
{
	//�t�H���g�R���N�V�����擾
	HRESULT hResult = pDWriteFactory_->GetSystemFontCollection(&pFontCollection_);

	UINT32 index;
	BOOL exists;
	hResult = pFontCollection_->FindFamilyName(fileName.c_str(), &index, &exists);

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

	// �V�����t�H�[�}�b�g���쐬
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
	outMetrics.ascentPx = static_cast<float>(fontMetrics_.ascent) * static_cast<float>(size) / static_cast<float>(fontMetrics_.designUnitsPerEm);
	outMetrics.descentPx = static_cast<float>(fontMetrics_.descent) * static_cast<float>(size) / static_cast<float>(fontMetrics_.designUnitsPerEm);
	outMetrics.lineGapPx = static_cast<float>(fontMetrics_.lineGap) * static_cast<float>(size) / static_cast<float>(fontMetrics_.designUnitsPerEm);
	//�e�L�X�g�̏�[���w����W�ɂقڂ҂����葵���邽�߂̃I�t�Z�b�g
	outMetrics.textTopOffset = -outMetrics.ascentPx + (outMetrics.ascentPx - outMetrics.descentPx) - outMetrics.lineGapPx;
}





void mtgb::DirectWrite::CreateTextLayout(const std::wstring& str, int size, IDWriteTextFormat* format, IDWriteTextLayout** ppTextLayout)
{
	//�e�L�X�g���C�A�E�g�쐬
	D2D1_SIZE_F rtSize = Game::System<Direct2D>().pDefRenderTarget_->GetSize();

	static const float dip = 96.0f;
	FLOAT dpiX, dpiY;
	Game::System<Direct2D>().pDefRenderTarget_->GetDpi(&dpiX, &dpiY);
	
	rtSize.width = rtSize.width / dpiX * dip;
	rtSize.height = rtSize.height / dpiY * dip;

	//IDWriteTextLayout* layout;
	HRESULT hResult = pDWriteFactory_->CreateTextLayout(str.data(), static_cast<uint32_t>(str.size()), format, rtSize.width, rtSize.height, ppTextLayout);

	massert(SUCCEEDED(hResult)
		&& "CreateTextLayout�Ɏ��s @DirectWrite::RegisterText");
}



void mtgb::DirectWrite::ChangeFormat(IDWriteTextFormat* format, mtgb::PixelFontMetrics& metrics)
{
	pTextFormat_ = format;
	pixelFontMetrics_ = metrics;
}



void mtgb::DirectWrite::Draw(IDWriteTextLayout* textLayout, float x, float y)
{
	D2D1_POINT_2F origin = { x, y  };

	Game::System<Direct2D>().pDefRenderTarget_->BeginDraw();
	Game::System<Direct2D>().pDefRenderTarget_->DrawTextLayout(origin, textLayout, Game::System<Direct2D>().pDefD2DBrush_.Get());
	Game::System<Direct2D>().pDefRenderTarget_->EndDraw();
}

void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text, float x, float y)
{
	/*D2D1_SIZE_F rtSize = mtgb::Direct2D::pDefRenderTarget_->GetSize();

	mtgb::Direct2D::pDefRenderTarget_->BeginDraw();

	mtgb::Direct2D::pDefRenderTarget_->DrawText(
		text.c_str(),
		static_cast<uint32_t>(text.length()),
		pTextFormat_,
		D2D1::RectF(x, y + pixelFontMetrics_.textTopOffset, rtSize.width, rtSize.height),
		mtgb::Direct2D::pDefD2DBrush_
	);

	mtgb::Direct2D::pDefRenderTarget_->EndDraw();*/
	ImmediateDraw(text, pTextFormat_, pixelFontMetrics_, x, y);
}





void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text,IDWriteTextFormat* format, const PixelFontMetrics& pixelFontMetrics, int x, int y)
{
	
	D2D1_SIZE_F rtSize = Game::System<Direct2D>().pDefRenderTarget_->GetSize();

	

	Game::System<Direct2D>().pDefRenderTarget_->BeginDraw();

	Game::System<Direct2D>().pDefRenderTarget_->DrawText(
		text.c_str(),
		static_cast<uint32_t>(text.length()),
		format,
		D2D1::RectF(
			static_cast<float>(x),
			y + pixelFontMetrics.textTopOffset, rtSize.width, rtSize.height),
		Game::System<Direct2D>().pDefD2DBrush_.Get()
	);

	Game::System<Direct2D>().pDefRenderTarget_->EndDraw();
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
