#pragma once
#include "IShader.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "RectInt.h"
#include "Color.h"

namespace mtgb
{
	class Figure : public IShader
	{
		/// <summary>
		/// 頂点情報
		/// </summary>
		struct Vertex
		{
			Vector3 position;  // 座標
		};

		/// <summary>
		/// GPUに送るやつ
		/// </summary>
		struct ConstantBuffer
		{
			Vector4 g_color;  // 色
			Matrix4x4 g_worldMatrix;  // 頂点座標変換行列
		};

	public:
		Figure();
		~Figure();

		void Draw(
			const RectInt& _rect,
			const Color& _color);

	private:
		/// <summary>
		/// 頂点バッファの初期化
		/// </summary>
		void InitializeVertexBuffer(ID3D11Device* _pDevice) override;
		/// <summary>
		/// インデックスバッファの初期化
		/// </summary>
		void InitializeIndexBuffer(ID3D11Device* _pDevice) override;
		/// <summary>
		/// コンスタントバッファ
		/// </summary>
		void InitializeConstantBuffer(ID3D11Device* _pDevice) override;
	};
}
