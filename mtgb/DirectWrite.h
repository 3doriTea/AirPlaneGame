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
	using TextLayoutKey = std::tuple<std::wstring, FontSize>; // 文字列とサイズのペア
	
	struct PixelFontMetrics
	{
		float ascentPx;
		float descentPx;
		float lineGapPx;
		float textTopOffset;
	};

	// フォントフォーマットのキャッシュエントリ
	struct FontFormatData
	{
		int fontSize;
		IDWriteTextFormat* format;
		PixelFontMetrics pixelFontMetrics;
		
		FontFormatData(int size, IDWriteTextFormat* fmt, const PixelFontMetrics& metrics)
			: fontSize(size), format(fmt), pixelFontMetrics(metrics) {}
		
		~FontFormatData();
	};

	// テキストレイアウトのキャッシュエントリ
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

	/// <summary>
	/// DirectWrite用のシステム
	/// </summary>
	class DirectWrite : public ISystem
	{
	public:
		DirectWrite();
		~DirectWrite();

		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize() override;
		/// <summary>
		/// 更新処理
		/// </summary>
		void Update() override;

		/// <summary>
		/// ハンドルに応じたテキストを描画
		/// 完全に同じ文字列を描画し続ける場合に適している
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="x">テキストの左端</param>
		/// <param name="y">テキストの上端</param>
		void Draw(int handle, float x, float y);

		void ImmediateDraw(const std::wstring& text, float x, float y);
		/// <summary>
		/// 即時描画
		/// 文字列だけが頻繁に変化する場合に適している
		/// </summary>
		/// <param name="text">描画するテキスト</param>
		/// <param name="x">テキストの左端</param>
		/// <param name="y">テキストの上端</param>
		void ImmediateDraw(const std::string& text, float x, float y);

		/// <summary>
		/// サイズを指定して即時描画
		/// サイズが異なるとIDWriteTextFormatを新しく作成してキャッシュするのでイージングなどには使わないでね
		/// </summary>
		/// <param name="text"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="size"></param>
		void ImmediateDraw(const std::string& text, float x, float y, int size);

		/// <summary>
		/// デフォルトフォントサイズを変更
		/// </summary>
		/// <param name="size">新しいデフォルトサイズ</param>
		void ChangeFontSize(int size);

		void Release();

		/// <summary>
		/// テキストを読み込んでそのハンドルを返す
		/// </summary>
		/// <param name="str">読み込むテキスト</param>
		/// <returns>描画時に渡すハンドル</returns>
		static int Load(const std::string& str);

		static int Load(const std::string& str, int size);

		static DWRITE_FONT_METRICS fontMetrics_;
		static PixelFontMetrics pixelFontMetrics_;
	private:
		/// <summary>
		/// Loadにハンドルを渡す
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		int RegisterText(const std::string& str);
		int RegisterText(const std::string& str, int size);

		/// <summary>
		/// 指定サイズのIDWriteTextFormatを取得または作成
		/// </summary>
		/// <param name="size">フォントサイズ</param>
		/// <returns>IDWriteTextFormatとPixelFontMetricsのペア</returns>
		std::pair<IDWriteTextFormat*, PixelFontMetrics> GetOrCreateTextFormat(int size);

		/// <summary>
		/// IDWriteTextFormatを作成
		/// </summary>
		/// <param name="size">フォントサイズ</param>
		/// <param name="ppTextFormat">作成されるIDWriteTextFormat</param>
		/// <param name="outMetrics">計算されるPixelFontMetrics</param>
		void CreateTextFormat(int size, IDWriteTextFormat** ppTextFormat, PixelFontMetrics& outMetrics);

		static IDWriteFactory* pDWriteFactory_;
		static IDWriteTextFormat* pTextFormat_; // デフォルトフォーマット
		static IDWriteTextLayout* pTextLayout_;
		static IDWriteFontCollection* pFontCollection_;
		static IDWriteFontFamily* pFontFamily_;
		static IDWriteFont* pDWriteFont_;
		
		TextLayoutDatas* textLayoutDatas_; // テキストレイアウトのキャッシュ
		FontFormatDatas* fontFormatDatas_; // フォントフォーマットのキャッシュ
		
		static int currentDefaultFontSize_; // 現在のデフォルトフォントサイズ
		int nextHandle_; // 次に割り当てるハンドル番号
	};
}
