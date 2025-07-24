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

// タグ定義
struct text_layout_order {}; // 文字列+サイズをキーとするタグ
struct handle_order {}; // ハンドルをキーとするタグ
struct random {}; // 添え字でアクセスするタグ
struct font_size_order {}; // フォントサイズをキーとするタグ

// テキストレイアウトの多重インデックスコンテナ
using TextLayoutDatas = multi_index_container<
	TextLayoutData*,
	indexed_by<
	// 文字列+サイズの複合キー
	ordered_unique<tag<text_layout_order>,
	composite_key<
	TextLayoutData,
	member<TextLayoutData, std::wstring, &TextLayoutData::str>,
	member<TextLayoutData, int, &TextLayoutData::fontSize>
	>
	>,
	// ハンドルをキー
	ordered_unique<tag<handle_order>,
	member<TextLayoutData, int, &TextLayoutData::handle>
	>,
	// 添え字でアクセス
	random_access<tag<random>>
	>
>;

// フォントフォーマットの多重インデックスコンテナ
using FontFormatDatas = multi_index_container<
	FontFormatData*,
	indexed_by<
	// フォントサイズをキー
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
		/// テキストを読み込んでそのハンドルを返す
		/// </summary>
		/// <param name="str">読み込むテキスト</param>
		/// <returns>描画時に渡すハンドル</returns>
		//static int Load(const std::string& str);

		/// <summary>
		/// テキストを読み込んでそのハンドルを返す
		/// サイズを指定しない場合デフォルト値に設定される
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="size">大きさ</param>
		/// <returns></returns>
		static int Load(const std::string& str, int size = currentDefaultFontSize_);

		/// <summary>
		/// ハンドルに応じたテキストを描画
		/// 完全に同じ文字列を描画し続ける場合に適している
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="x">テキストの左端</param>
		/// <param name="y">テキストの上端</param>
		void Draw(int handle, float x, float y);

		/// <summary>
		/// 即時描画
		/// 文字列だけが頻繁に変化する場合に適している
		/// </summary>
		/// <param name="text">描画するテキスト</param>
		/// <param name="x">テキストの左端</param>
		/// <param name="y">テキストの上端</param>
		void ImmediateDraw(const std::string& text, float x, float y,int size = currentDefaultFontSize_);

		//void ImmediateDraw(const std::wstring& text, float x, float y);
		//void ImmediateDraw(const std::string& text, float x, float y);

		/// <summary>
		/// サイズを指定して即時描画
		/// サイズが異なるとIDWriteTextFormatを新しく作成してキャッシュするのでイージングなどには使わないでね
		/// </summary>
		/// <param name="text"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="size"></param>
		void ImmediateDraw(const std::wstring& text, float x, float y,int size = currentDefaultFontSize_);

		/// <summary>
		/// 以降のデフォルトのフォントサイズを変更する
		/// </summary>
		/// <param name="size"></param>
		void ChangeFontSize(int size);
	private:

		/// <summary>
		/// 新規または既存のテキストのハンドルを返す
		/// </summary>
		/// <param name="text"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		static int GetOrCreateTextLayout(const std::wstring& text, int size);

		/// <summary>
		/// 指定サイズのIDWriteTextFormatを取得または作成
		/// </summary>
		/// <param name="size">フォントサイズ</param>
		/// <returns>IDWriteTextFormatとPixelFontMetricsのペア</returns>
		static std::pair<IDWriteTextFormat*, PixelFontMetrics> GetOrCreateTextFormat(int size);

		//現在のデフォルトのフォントサイズ
		static int currentDefaultFontSize_;

		//デフォルトのフォントファミリー
		const static wchar_t* DEFAULT_FONT_FAMILY_NAME;

		static TextLayoutDatas* textLayoutDatas_; // テキストレイアウトのキャッシュ
		static FontFormatDatas* fontFormatDatas_; // フォントフォーマットのキャッシュ

		static int nextHandle_; // 次に割り当てるハンドル番号
		
	};
}

