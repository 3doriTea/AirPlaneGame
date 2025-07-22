#pragma once
#include <cmtgb.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <tuple>
#include <cmath>

#include "ReleaseUtility.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>


using namespace boost::multi_index;

struct IDWriteFactory;
struct IDWriteTextFormat;
struct IDWriteTextLayout;
struct IDWriteFontCollection;
struct IDWriteFontFamily;
struct IDWriteFont;
struct DWRITE_FONT_METRICS;

namespace mtgb
{
	using FontName = std::wstring;
	using FontSize = int;
	using TextLayoutKey = std::tuple<std::wstring, FontSize>; // ������ƃT�C�Y�̃y�A
	
	struct PixelFontMetrics
	{
		float ascentPx;
		float descentPx;
		float lineGapPx;
		float textTopOffset;
	};

	// �t�H���g�t�H�[�}�b�g�̃L���b�V���G���g��
	struct FontFormatData
	{
		int fontSize;
		IDWriteTextFormat* format;
		PixelFontMetrics pixelFontMetrics;
		
		FontFormatData(int size, IDWriteTextFormat* fmt, const PixelFontMetrics& metrics)
			: fontSize(size), format(fmt), pixelFontMetrics(metrics) {}
		
		~FontFormatData();
	};

	// �e�L�X�g���C�A�E�g�̃L���b�V���G���g��
	struct TextLayoutData
	{
		std::wstring str;
		int fontSize;
		IDWriteTextLayout* layout;
		int handle;
		
		TextLayoutData(const std::wstring& text, int size, IDWriteTextLayout* lay, int h)
			: str(text), fontSize(size), layout(lay), handle(h) {}
		
		~TextLayoutData();
	};

	// �^�O��`
	struct text_layout_order {}; // ������+�T�C�Y���L�[�Ƃ���^�O
	struct handle_order {}; // �n���h�����L�[�Ƃ���^�O
	struct random {}; // �Y�����ŃA�N�Z�X����^�O
	struct font_size_order {}; // �t�H���g�T�C�Y���L�[�Ƃ���^�O

	// �e�L�X�g���C�A�E�g�̑��d�C���f�b�N�X�R���e�i
	using TextLayoutDatas = multi_index_container<
		TextLayoutData*,
		indexed_by<
			// ������+�T�C�Y�̕����L�[
			ordered_unique<tag<text_layout_order>,
				composite_key<
					TextLayoutData,
					member<TextLayoutData, std::wstring, &TextLayoutData::str>,
					member<TextLayoutData, int, &TextLayoutData::fontSize>
				>
			>,
			// �n���h�����L�[
			ordered_unique<tag<handle_order>,
				member<TextLayoutData, int, &TextLayoutData::handle>
			>,
			// �Y�����ŃA�N�Z�X
			random_access<tag<random>>
		>
	>;

	// �t�H���g�t�H�[�}�b�g�̑��d�C���f�b�N�X�R���e�i
	using FontFormatDatas = multi_index_container<
		FontFormatData*,
		indexed_by<
			// �t�H���g�T�C�Y���L�[
			ordered_unique<tag<font_size_order>,
				member<FontFormatData, int, &FontFormatData::fontSize>
			>
		>
	>;

	/// <summary>
	/// DirectWrite�p�̃V�X�e��
	/// </summary>
	class DirectWrite : public ISystem
	{
	public:
		DirectWrite();
		~DirectWrite();

		/// <summary>
		/// ����������
		/// </summary>
		void Initialize() override;
		/// <summary>
		/// �X�V����
		/// </summary>
		void Update() override;

		/// <summary>
		/// �n���h���ɉ������e�L�X�g��`��
		/// ���S�ɓ����������`�悵������ꍇ�ɓK���Ă���
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="x">�e�L�X�g�̍��[</param>
		/// <param name="y">�e�L�X�g�̏�[</param>
		void Draw(int handle, float x, float y);

		void ImmediateDraw(const std::wstring& text, float x, float y);
		/// <summary>
		/// �����`��
		/// �����񂾂����p�ɂɕω�����ꍇ�ɓK���Ă���
		/// </summary>
		/// <param name="text">�`�悷��e�L�X�g</param>
		/// <param name="x">�e�L�X�g�̍��[</param>
		/// <param name="y">�e�L�X�g�̏�[</param>
		void ImmediateDraw(const std::string& text, float x, float y);

		/// <summary>
		/// �T�C�Y���w�肵�đ����`��
		/// �T�C�Y���قȂ��IDWriteTextFormat��V�����쐬���ăL���b�V������̂ŃC�[�W���O�Ȃǂɂ͎g��Ȃ��ł�
		/// </summary>
		/// <param name="text"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="size"></param>
		void ImmediateDraw(const std::string& text, float x, float y, int size);

		/// <summary>
		/// �f�t�H���g�t�H���g�T�C�Y��ύX
		/// </summary>
		/// <param name="size">�V�����f�t�H���g�T�C�Y</param>
		void ChangeFontSize(int size);

		void Release();

		/// <summary>
		/// �e�L�X�g��ǂݍ���ł��̃n���h����Ԃ�
		/// </summary>
		/// <param name="str">�ǂݍ��ރe�L�X�g</param>
		/// <returns>�`�掞�ɓn���n���h��</returns>
		static int Load(const std::string& str);

		static int Load(const std::string& str, int size);

		static DWRITE_FONT_METRICS fontMetrics_;
		static PixelFontMetrics pixelFontMetrics_;
	private:
		/// <summary>
		/// Load�Ƀn���h����n��
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		int RegisterText(const std::string& str);
		int RegisterText(const std::string& str, int size);

		/// <summary>
		/// �w��T�C�Y��IDWriteTextFormat���擾�܂��͍쐬
		/// </summary>
		/// <param name="size">�t�H���g�T�C�Y</param>
		/// <returns>IDWriteTextFormat��PixelFontMetrics�̃y�A</returns>
		std::pair<IDWriteTextFormat*, PixelFontMetrics> GetOrCreateTextFormat(int size);

		/// <summary>
		/// IDWriteTextFormat���쐬
		/// </summary>
		/// <param name="size">�t�H���g�T�C�Y</param>
		/// <param name="ppTextFormat">�쐬�����IDWriteTextFormat</param>
		/// <param name="outMetrics">�v�Z�����PixelFontMetrics</param>
		void CreateTextFormat(int size, IDWriteTextFormat** ppTextFormat, PixelFontMetrics& outMetrics);

		static IDWriteFactory* pDWriteFactory_;
		static IDWriteTextFormat* pTextFormat_; // �f�t�H���g�t�H�[�}�b�g
		static IDWriteTextLayout* pTextLayout_;
		static IDWriteFontCollection* pFontCollection_;
		static IDWriteFontFamily* pFontFamily_;
		static IDWriteFont* pDWriteFont_;
		
		TextLayoutDatas* textLayoutDatas_; // �e�L�X�g���C�A�E�g�̃L���b�V��
		FontFormatDatas* fontFormatDatas_; // �t�H���g�t�H�[�}�b�g�̃L���b�V��
		
		static int currentDefaultFontSize_; // ���݂̃f�t�H���g�t�H���g�T�C�Y
		int nextHandle_; // ���Ɋ��蓖�Ă�n���h���ԍ�
	};
}
