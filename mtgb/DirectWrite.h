#pragma once
#include <cmtgb.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <tuple>
#include <cmath>

#include "RectInt.h"
#include "ReleaseUtility.h"
#include "TextAlignment.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct IDWriteFactory;
struct IDWriteTextFormat;
struct IDWriteTextLayout;
struct IDWriteFontCollection;
struct IDWriteFontFamily;
struct IDWriteFont;
struct DWRITE_FONT_METRICS;
struct ID2D1Brush;

namespace mtgb
{
	//using TextLayoutKey = std::tuple<std::wstring, int>; // 文字列とサイズのペア
	
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
		ComPtr<IDWriteTextFormat> format;
		PixelFontMetrics pixelFontMetrics;
		
		FontFormatData(int size, IDWriteTextFormat* fmt, const PixelFontMetrics& metrics);
		
		~FontFormatData();
	};

	// テキストレイアウトのキャッシュエントリ
	struct TextLayoutData
	{
		std::wstring str;
		int fontSize;
		float width, height;

		ComPtr<IDWriteTextLayout> layout;
		int handle;
		
		TextLayoutData(const std::wstring& _str, int _size, float _width, float _height, IDWriteTextLayout* _layout, int _handle);
		
		~TextLayoutData();
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


		void CreateFontFormatData(const std::wstring& fileName,int fontSize, FontFormatData** ppFontFormatData);
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
		//void Draw(int handle, float x, float y);

		void Draw(ComPtr<IDWriteTextLayout> textLayout, float x, float y);


		/// <summary>
		/// 即時描画
		/// 文字列だけが頻繁に変化する場合に適している
		/// </summary>
		/// <param name="text">描画するテキスト</param>
		/// <param name="x">テキストの左端</param>
		/// <param name="y">テキストの上端</param>
		void ImmediateDraw(const std::wstring& text, float x, float y);
		

		
		//void ImmediateDraw(const std::string& text, float x, float y, int size);
		void ImmediateDraw(const std::wstring& text, ComPtr<IDWriteTextFormat> format, const PixelFontMetrics& pixelFontMetrics, float x,float y,float width,float height);

		void ImmediateDraw(const std::wstring& text, ComPtr<IDWriteTextFormat> format, const PixelFontMetrics& pixelFontMetrics, float x, float y);
		
		/// <summary>
		/// デフォルトフォントサイズを変更
		/// </summary>
		/// <param name="size">新しいデフォルトサイズ</param>
		//void ChangeFontSize(int size);

		void ChangeFormat(ComPtr<IDWriteTextFormat> format, mtgb::PixelFontMetrics& metrics);

		void Release() override;

		

		
		
		/// <summary>
		/// IDWriteTextLayoutを作成
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="size">大きさ</param>
		/// <param name="format"></param>
		/// <param name="ppTextLayout">作成されるIDWriteTextLayout</param>
		void CreateTextLayout(const std::wstring & _str, int  _size, ComPtr<IDWriteTextFormat> _format, IDWriteTextLayout ** _ppTextLayout);
		void CreateTextLayout(const std::wstring & _str, float  _width, float _height, int  _size, ComPtr<IDWriteTextFormat> _format, IDWriteTextLayout ** _ppTextLayout);

		

		/// <summary>
		/// IDWriteTextFormatを作成
		/// </summary>
		/// <param name="size">フォントサイズ</param>
		/// <param name="ppTextFormat">作成されるIDWriteTextFormat</param>
		/// <param name="outMetrics">計算されるPixelFontMetrics</param>
		void CreateTextFormat(int size, IDWriteTextFormat** ppTextFormat, PixelFontMetrics& outMetrics);

		void SetTextAlignment(TextAlignment alignment, ComPtr<IDWriteTextFormat> format);
	private:
	
		static DWRITE_FONT_METRICS fontMetrics_;
		static PixelFontMetrics pixelFontMetrics_;
		static ComPtr<IDWriteFactory> pDWriteFactory_;
		static ComPtr<IDWriteTextFormat> pTextFormat_; // デフォルトフォーマット
		//static IDWriteTextLayout* pTextLayout_;
		static ComPtr<IDWriteFontCollection> pFontCollection_;
		static ComPtr<IDWriteFontFamily> pFontFamily_;
		static ComPtr<IDWriteFont> pDWriteFont_;
		
		//static int currentDefaultFontSize_; // 現在のデフォルトフォントサイズ
	};
}
