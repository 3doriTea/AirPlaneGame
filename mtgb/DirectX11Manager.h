#pragma once
#include "ISystem.h"
#include <string>

#pragma comment(lib, "d3d11.lib")  // DirectX11のライブラリ
#pragma comment(lib, "d3dcompiler.lib")  // シェーダコンパイラ用ライブラリ

struct D3D11_INPUT_ELEMENT_DESC;
struct CD3D11_RASTERIZER_DESC;

namespace mtgb
{
	enum struct ShaderType : int8_t;

	class DirectX11Manager : public ISystem
	{
	public:
		DirectX11Manager();
		~DirectX11Manager();

		void Initialize() override;
		void Update() override;

	private:
		/// <summary>
		/// シェーダバンドルを初期化する
		/// </summary>
		void InitializeShaderBundle();

		/// <summary>
		/// シェーダファイルをコンパイルする
		/// </summary>
		/// <param name="_fileName">ファイルパス</param>
		/// <param name="_type">シェーダタイプ</param>
		/// <param name="_pHLSLLayout">入力情報配列</param>
		/// <param name="_layoutLength">入力情報配列の要素数</param>
		/// <param name="_pRasterizerDesc">ラスタライザの設定</param>
		void CompileShader(
			const std::wstring& _fileName,
			const ShaderType& _type,
			const D3D11_INPUT_ELEMENT_DESC* _pHLSLLayout,
			const unsigned int _layoutLength,
			const CD3D11_RASTERIZER_DESC* _pRasterizerDesc);
	};
}
