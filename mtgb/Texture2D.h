#pragma once
#include <string>
#include "Vector2Int.h"

#pragma comment(lib, "WindowsCodecs.lib")

struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct D3D11_TEXTURE2D_DESC;

namespace mtgb
{
	/// <summary>
	/// 2Dのテクスチャ
	/// </summary>
	class Texture2D
	{
	public:
		Texture2D();
		~Texture2D();

		/// <summary>
		/// 画像ファイルからテクスチャをロードする
		/// </summary>
		/// <param name="fileName">ファイルのパス</param>
		void Load(const std::wstring& _fileName);

		/// <summary>
		/// サンプラーステートを取得
		/// </summary>
		/// <returns>サンプラーステートのポインタ</returns>
		inline ID3D11SamplerState* GetSamplerState() { return pSamplerLinear_; }
		/// <summary>
		/// シェーダリソースビュを取得
		/// </summary>
		/// <returns>シェーダリソースビュのポインタ</returns>
		inline ID3D11ShaderResourceView* GetShaderResourceView() { return pShaderResourceView_; }
	
		/// <summary>
		/// テクスチャの画像のサイズを取得
		/// </summary>
		/// <returns>ヨコ幅とタテ幅のピクセル</returns>
		inline const Vector2Int GetSize() const { return size_; }
	
	private:
		ID3D11SamplerState* pSamplerLinear_;  // 貼り方
		ID3D11ShaderResourceView* pShaderResourceView_;  // シェーダに送るためのもの
		Vector2Int size_;  // 画像のサイズ
	};
}
