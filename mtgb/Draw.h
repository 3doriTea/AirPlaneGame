#pragma once
#include "Vector2Int.h"
#include "RectInt.h"
#include "Color.h"
#include "ISystem.h"
#include "ShaderType.h"
#include "TextAlignment.h"

namespace mtgb
{
	class Figure;
	class Transform;
	class FbxModel;
	class Ground;
	//enum struct ShaderType : int8_t;
	
	/// <summary>
	/// 描画する系
	/// </summary>
	class Draw final : public ISystem
	{
	public:
		enum struct Pivot
		{
			TopLeft,
			Center,
			BottomMiddle,
		};

	public:
		static void SetShaderOnce(const ShaderType _type) { Draw::onceShaderType_ = _type; }
		/// <summary>
		/// シェーダがセットされているかチェックする
		/// </summary>
		/// <param name="_default">セットされていない場合のデフォルトシェーダ</param>
		static void CheckSetShader(const ShaderType _default);

		static void Box(
			const Vector2Int& _begin,
			const Vector2Int& _end,
			const Color& _color);

		static void Box(
			const RectInt& _rect,
			const Color& _color);

		/*static void Image(
			const Vector2Int& _position,
			const ImageHandle _hImage);

		static void Image(
			const RectInt& _draw,
			const ImageHandle _hImage);*/

		static void Image(
			const ImageHandle _hImage,
			const RectInt& _draw,
			const RectInt& _cut,
			const float _rotationZ);

		static void Image(
			const ImageHandle _hImage,
			const Transform* _pTransform);

		static void Model(
			const ModelHandle _hModel,
			const Transform* _pTransform);

		static void OBJModel(
			const OBJModelHandle _hOBJModel,
			const Transform* _pTransform);

		static void FBXModel(
			const FBXModelHandle _hFBXModel,
			const Transform& _pTransform,
			const int _frame);

		/// <summary>
		/// <para> 事前にLoadをしたテキストを描画 </para>
		/// <para> 文字列内容、サイズが変化しない場合に適している </para>
		/// <para> 矩形領域に描画される 幅と高さはLoad時に指定したもの </para>
		/// </summary>
		/// <param name="_hText">テキストのハンドル</param>
		/// <param name="origin">矩形領域の左上</param>
		/// <param name="alignment">テキストの配置</param>
		static void Text(
			const TextHandle _hText, 
			const Vector2Int& origin,
			TextAlignment alignment = currentDefaultTextAlignment_);

		static void Text(
			const TextHandle _hText, 
			float x, float y,
			TextAlignment alignment = currentDefaultTextAlignment_);

		/// <summary>
		/// 頻繫に変わるテキストを描画する
		/// </summary>
		/// <param name="text">描画する文字列</param>
		/// <param name="x">描画座標ピクセル x</param>
		/// <param name="y">描画座標ピクセル y</param>
		/*static void ImmediateText(
			const std::string& text,
			float x,
			float y
		);*/

		/// <summary>
		/// <para> 文字列内容が頻繁に変化するテキスト(タイマーやスコアなど)を即時に描画</para>
		/// <para> 矩形領域に描画される 幅と高さはウィンドウのサイズ </para>
		/// </summary>
		/// <param name="text">描画する文字列</param>
		/// <param name="x">矩形領域の左端</param>
		/// <param name="y">矩形領域の上端</param>
		/// <param name="size">テキストのフォントサイズ（省略時は設定中のサイズ）</param>
		/// <param name="alignment">テキストの配置（省略時は設定中の配置）</param>
		static void ImmediateText(
			const std::string& text,
			float x,
			float y,
			int size = currentDefaultFontSize_,
			TextAlignment alignment = currentDefaultTextAlignment_
		);
		
		/// <summary>
		/// <para> 文字列内容が頻繁に変化するテキスト(タイマーやスコアなど)を即時に描画</para>
		/// <para> 矩形領域に描画される </para>
		/// </summary>
		/// <param name="text">描画する文字列</param>
		/// <param name="rect">矩形</param>
		/// <param name="size">テキストのフォントサイズ（省略時は設定中のサイズ）</param>
		/// <param name="alignment">テキストの配置（省略時は設定中の配置）</param>
		static void ImmediateText(
			const std::string& text,
			RectInt rect,
			int size = currentDefaultFontSize_,
			TextAlignment alignment = currentDefaultTextAlignment_
		);

		/// <summary>
		/// <para> 文字列内容が頻繁に変化するテキスト(タイマーやスコアなど)を即時に描画</para>
		/// <para> 矩形領域に描画される </para>
		/// </summary>
		/// <param name="text">描画する文字列</param>
		/// <param name="x">矩形領域の左端</param>
		/// <param name="y">矩形領域の上端</param>
		/// <param name="width">矩形領域の幅</param>
		/// <param name="height">矩形領域の幅</param>
		/// <param name="size">テキストのフォントサイズ（省略時は設定中のサイズ）</param>
		/// <param name="alignment">テキストの配置（省略時は設定中の配置）</param>
		static void ImmediateText(
			const std::string& text,
			float x, float y,float width,float height,
			int size = currentDefaultFontSize_,
			TextAlignment alignment = currentDefaultTextAlignment_
		);

		/// <summary>
		/// テキストの配置を設定
		/// 以降テキスト描画時に省略すると適用される
		/// </summary>
		/// <param name="_alignment">指定する配置方法</param>
		static void ChangeTextAlignment(TextAlignment _alignment);
		/// <summary>
		/// フォント、というか文字のサイズを設定
		/// 以降テキスト描画時に省略すると適用される
		/// </summary>
		/// <param name="size"></param>
		static void ChangeFontSize(int size);
		static void GroundPlane();

	public:
		Draw();
		~Draw();
		void Initialize() override;
		void Update() override;

	private:
		FbxModel* pFbxModel_;
		Figure* pFigure_;
		Ground* pGround_;
		static int currentDefaultFontSize_;
		static TextAlignment currentDefaultTextAlignment_;
		static ShaderType onceShaderType_;
	};
}
