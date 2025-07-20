#pragma once
#include <cmtgb.h>
#include <string>
#include "ReleaseUtility.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

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
	struct TextLayoutEntry
	{
		std::wstring text;
		IDWriteTextLayout* layout;
		int handle;
		//TextLayoutEntry() = default;
		~TextLayoutEntry();
	};


	struct order {};//辞書順のタグ
	struct random {};//添え字でアクセスするタグ

	using TextLayoutSet = multi_index_container <
		TextLayoutEntry*,
		indexed_by<
		ordered_unique<::tag<order>,
		member<TextLayoutEntry, std::wstring, &TextLayoutEntry::text>>,
		random_access<tag<random>>
		>
		>;

	struct PixelFontMetrics
	{
		float ascentPx;
		float descentPx;
		float lineGapPx;
		float textTopOffset;
	};

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


		void Draw(int handle, float x, float y);

		void Release();

		int RegisterText(const std::string& str);

		static int Load(const std::string& str);

		static DWRITE_FONT_METRICS fontMetrics_;
		static PixelFontMetrics pixelFontMetrics_;
	private:
		static IDWriteFactory* pDWriteFactory_;
		static IDWriteTextFormat* pTextFormat_;
		static IDWriteTextLayout* pTextLayout_;
		static IDWriteFontCollection* pFontCollection_;
		static IDWriteFontFamily* pFontFamily_;
		static IDWriteFont* pDWriteFont_;
		TextLayoutSet* textLayouts_;
	};
}
