#pragma once
#include "IShader.h"
#include "../Source/Library/YzTrailEmitter.h"

namespace mtgb
{
	class Trail : public IShader
	{
	private:
		struct Vertex
		{
			Vector3 position_;
			Vector4 color_;
		};

		struct ConstantBuffer
		{
			Matrix4x4 wvp_;
		};

	public:
		Trail();
		~Trail();

		void Draw(const yz::TrailEmitter& _trailEmmiter);

		/// <summary>
		/// 頂点バッファの初期化
		/// </summary>
		void InitializeVertexBuffer(ID3D11Device* _pDevice);
		/// <summary>
		/// インデックスバッファの初期化
		/// </summary>
		void InitializeIndexBuffer(ID3D11Device* _pDevice);
		/// <summary>
		/// コンスタントバッファの初期化
		/// </summary>
		void InitializeConstantBuffer(ID3D11Device* _pDevice);
	};
}
