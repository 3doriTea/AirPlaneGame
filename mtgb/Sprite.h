#pragma once
#include "IShader.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector2.h"
#include "RectInt.h"
#include "Color.h"
#include <string>
#include "Texture2D.h"


namespace mtgb
{
	class Transform;

	class Sprite : public IShader
	{
		/// <summary>
		/// 頂点情報
		/// </summary>
		struct Vertex
		{
			Vector3 position;
			Vector3 uv;
		};

		/// <summary>
		/// GPUに送るやつ
		/// </summary>
		struct ConstantBuffer
		{
			Matrix4x4 g_matrixCameraTranslate;          // カメラ座標変換行列
			Matrix4x4 g_matrixCameraRotation;          // カメラ座標変換行列
			Matrix4x4 g_matrixWorldTranslate;  // 頂点座標変換行列
			Matrix4x4 g_matrixWorldRotation;   // 頂点座標変換行列
			Matrix4x4 g_matrixTexture;         // テクスチャ座標変換行列
			Vector4 g_color;                   // 色
			Vector2 g_angle;                   // 角度
		};

	public:
		Sprite();
		~Sprite();

		/// <summary>
		/// 画像を読み込む
		/// </summary>
		/// <param name="_fileName">画像のファイル名</param>
		void Load(const std::wstring& _fileName);
		/// <summary>
		/// Spriteを描画する
		/// </summary>
		/// <param name="_draw">描画する矩形</param>
		/// <param name="_cut">画像内のトリミング矩形</param>
		/// <param name="_rotationZ">回転角度(Degree)</param>
		/// <param name="_color">描画色</param>
		void Draw(
			const RectInt& _draw,
			const float _rotationZ,
			const RectInt& _cut,
			const Color& _color);

		/// <summary>
		/// Transformをもとに描画する
		/// </summary>
		/// <param name="_pTransform">画像のTransform</param>
		/// <param name="_pCameraTransform">
		/// <para>カメラのTransform</para>
		/// <para>nullptr指定可能</para>
		/// </param>
		/// <param name="_imageSize">画像サイズ</param>
		/// <param name="_color">色</param>
		void Draw(
			const Transform* _pTransform,
			const Transform* _pCameraTransform,
			const Vector2Int& _imageSize,
			const Color& _color);

		/// <summary>
		/// 読み込まれた画像のサイズを取得
		/// </summary>
		/// <returns></returns>
		inline const Vector2Int GetSize() const { return texture2D_.GetSize(); }

	private:
		void InitializeVertexBuffer(ID3D11Device* _pDevice) override;
		void InitializeIndexBuffer(ID3D11Device* _pDevice) override;
		void InitializeConstantBuffer(ID3D11Device* _pDevice) override;

	private:
		Texture2D texture2D_;  // 2Dのテクスチャ
	};
}
