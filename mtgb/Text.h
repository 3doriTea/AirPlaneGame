#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <string>
#include "DirectWrite.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

using namespace boost::multi_index;
using namespace mtgb;

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

namespace mtgb
{
	class Text : public ISystem
	{
	public:
		Text();
		~Text();

		void Initialize() override;
		void Update() override;

		void Release();
		/// <summary>
		/// �e�L�X�g��ǂݍ���ł��̃n���h����Ԃ�
		/// </summary>
		/// <param name="str">�ǂݍ��ރe�L�X�g</param>
		/// <returns>�`�掞�ɓn���n���h��</returns>
		//static int Load(const std::string& str);

		/// <summary>
		/// �e�L�X�g��ǂݍ���ł��̃n���h����Ԃ�
		/// �T�C�Y���w�肵�Ȃ��ꍇ�f�t�H���g�l�ɐݒ肳���
		/// </summary>
		/// <param name="str">������</param>
		/// <param name="size">�傫��</param>
		/// <returns></returns>
		static int Load(const std::string& str, int size = currentDefaultFontSize_);

		/// <summary>
		/// �n���h���ɉ������e�L�X�g��`��
		/// ���S�ɓ����������`�悵������ꍇ�ɓK���Ă���
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="x">�e�L�X�g�̍��[</param>
		/// <param name="y">�e�L�X�g�̏�[</param>
		void Draw(int handle, float x, float y);

		/// <summary>
		/// �����`��
		/// �����񂾂����p�ɂɕω�����ꍇ�ɓK���Ă���
		/// </summary>
		/// <param name="text">�`�悷��e�L�X�g</param>
		/// <param name="x">�e�L�X�g�̍��[</param>
		/// <param name="y">�e�L�X�g�̏�[</param>
		void ImmediateDraw(const std::string& text, float x, float y,int size = currentDefaultFontSize_);

		//void ImmediateDraw(const std::wstring& text, float x, float y);
		//void ImmediateDraw(const std::string& text, float x, float y);

		/// <summary>
		/// �T�C�Y���w�肵�đ����`��
		/// �T�C�Y���قȂ��IDWriteTextFormat��V�����쐬���ăL���b�V������̂ŃC�[�W���O�Ȃǂɂ͎g��Ȃ��ł�
		/// </summary>
		/// <param name="text"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="size"></param>
		void ImmediateDraw(const std::wstring& text, float x, float y,int size = currentDefaultFontSize_);

		/// <summary>
		/// �ȍ~�̃f�t�H���g�̃t�H���g�T�C�Y��ύX����
		/// </summary>
		/// <param name="size"></param>
		void ChangeFontSize(int size);
	private:

		/// <summary>
		/// �V�K�܂��͊����̃e�L�X�g�̃n���h����Ԃ�
		/// </summary>
		/// <param name="text"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		static int GetOrCreateTextLayout(const std::wstring& text, int size);

		/// <summary>
		/// �w��T�C�Y��IDWriteTextFormat���擾�܂��͍쐬
		/// </summary>
		/// <param name="size">�t�H���g�T�C�Y</param>
		/// <returns>IDWriteTextFormat��PixelFontMetrics�̃y�A</returns>
		static std::pair<IDWriteTextFormat*, PixelFontMetrics> GetOrCreateTextFormat(int size);

		//���݂̃f�t�H���g�̃t�H���g�T�C�Y
		static int currentDefaultFontSize_;

		//�f�t�H���g�̃t�H���g�t�@�~���[
		const static wchar_t* DEFAULT_FONT_FAMILY_NAME;

		static TextLayoutDatas* textLayoutDatas_; // �e�L�X�g���C�A�E�g�̃L���b�V��
		static FontFormatDatas* fontFormatDatas_; // �t�H���g�t�H�[�}�b�g�̃L���b�V��

		static int nextHandle_; // ���Ɋ��蓖�Ă�n���h���ԍ�
		
	};
}

