#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <string>
#include "DirectWrite.h"
#include "TextAlignment.h"
#include "Vector2F.h"

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
struct layout_box_size_order {};// レイアウトボックスのサイズをキーとするタグ

// テキストレイアウトの多重インデックスコンテナ
using TextLayoutDatas = multi_index_container<
	TextLayoutData*,
	indexed_by<
	// 文字列+サイズの複合キー（複数のレイアウトボックスで同じ場合があるので non_unique）
	ordered_non_unique<tag<text_layout_order>,
	composite_key<TextLayoutData,
	member<TextLayoutData, std::wstring, &TextLayoutData::str>,
	member<TextLayoutData, int, &TextLayoutData::fontSize>>>,

	// レイアウトボックスの幅+高さの複合キー（複数テキストで同じ場合があるので non_unique）
	ordered_non_unique<tag<layout_box_size_order>,
	composite_key<TextLayoutData,
	member<TextLayoutData,float,&TextLayoutData::width>,
	member<TextLayoutData, float, &TextLayoutData::height>>>,

	// ハンドルをキーとする
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
		friend class Draw;
	public:
		Text();
		~Text();

		void Initialize() override;
		void Update() override;

		void Release();
		
		/// <summary>
		/// <para> テキストを読み込んでそのハンドルを返す </para>
		/// <para> 文字列内容、サイズが変化しない場合に適している </para>
		/// </summary>
		/// <param name="str">読み込む文字列</param>
		/// <param name="size">テキストのフォントサイズ</param>
		/// <returns></returns>
		static int Load(const std::string& str, int size);

		/// <summary>
		/// <para> テキストを読み込んでそのハンドルを返す </para>
		/// <para> 文字列内容、サイズが変化しない場合に適している </para>
		/// <para> 描画される矩形領域の幅と高さを予め指定する</para>
		/// </summary>
		/// <param name="str">読み込む文字列</param>
		/// <param name="fontSize">テキストのフォントサイズ</param>
		/// <param name="layoutBoxWidth">矩形領域の幅</param>
		/// <param name="layoutBoxHeight">矩形領域の高さ</param>
		/// <returns></returns>
		static int Load(const std::string& str, int fontSize, float layoutBoxWidth, float layoutBoxHeight);

		/// <summary>
		/// <para> テキストを読み込んでそのハンドルを返す </para>
		/// <para> 文字列内容、サイズが変化しない場合に適している </para>
		/// <para> 描画される矩形領域の幅と高さを予め指定する</para>
		/// </summary>
		/// <param name="str">読み込む文字列</param>
		/// <param name="fontSize">テキストのフォントサイズ</param>
		/// <param name="layoutBoxSize">矩形領域の幅、高さ</param>
		/// <returns></returns>
		static int Load(const std::string& str, int fontSize,Vector2F layoutBoxSize);
		

#if false
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
		void ImmediateDraw(const std::string& text, float x, float y);
		void ImmediateDraw(const std::string& text, float x, float y,int size);
		void ImmediateDraw(const std::string& text, RectInt rect,int size);


		//void ImmediateDraw(const std::wstring& text, float x, float y);

		/// <summary>
		/// サイズを指定して即時描画
		/// サイズが異なるとIDWriteTextFormatを新しく作成してキャッシュするのでイージングなどには使わないでね
		/// </summary>
		/// <param name="text"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="size"></param>
		void ImmediateDraw(const std::wstring& text, float x, float y,int size);
		void ImmediateDraw(const std::wstring& text, float x, float y);
		void ImmediateDraw(const std::wstring& text, float x,float y,float width,float height,int size);


		/// <summary>
		/// 以降のデフォルトのフォントサイズを変更する
		/// </summary>
		/// <param name="size"></param>
		void ChangeFontSize(int size);

		void ChangeTextAlignment(TextAlignment alignment);
#endif
	private:

		/// <summary>
		/// 新規または既存のテキストのハンドルを返す
		/// </summary>
		/// <param name="text"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		int GetOrCreateTextLayoutHandle(const std::wstring& text, int size);
		int GetOrCreateTextLayoutHandle(const std::wstring& text, int size,float width,float height);

		TextLayoutData* GetTextLayoutData(int handle);

		/// <summary>
		/// 指定サイズのIDWriteTextFormatを取得または作成
		/// </summary>
		/// <param name="size">フォントサイズ</param>
		/// <returns>IDWriteTextFormatとPixelFontMetricsのペア</returns>
		FontFormatData* GetOrCreateTextFormat(int size);

		//現在のデフォルトのフォントサイズ
		int currentDefaultFontSize_;

		TextAlignment currentTextAlignment_;

		//デフォルトのフォントファミリー
		const static wchar_t* DEFAULT_FONT_FAMILY_NAME;

		TextLayoutDatas* textLayoutDatas_; // テキストレイアウトのキャッシュ
		FontFormatDatas* fontFormatDatas_; // フォントフォーマットのキャッシュ

		int nextHandle_; // 次に割り当てるハンドル番号
		
	};
}

