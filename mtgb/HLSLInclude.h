#pragma once
#include <d3dcompiler.h>

namespace mtgb
{
	/// <summary>
	/// シェーダヘッダ読み込み用 - 継承している
	/// </summary>
	struct HLSLInclude : public ID3DInclude
	{
		/// <summary>
		/// 開くときの処理
		/// </summary>
		/// <param name="_includeType">インクルードのタイプ</param>
		/// <param name="_pFileNamm">読み込むファイルパス</param>
		/// <param name="_pParentData">親データのポインタ渡し</param>
		/// <param name="_ppData">データのポインタのポインタ渡し</param>
		/// <param name="_pBytes">バイト数のポインタ渡し</param>
		/// <returns>結果ハンドル</returns>
		HRESULT Open(
			D3D_INCLUDE_TYPE _includeType,
			LPCSTR _pFileName,
			LPCVOID _pParentData,
			LPCVOID* _ppData,
			UINT* _pBytes) override;

		/// <summary>
		/// 閉じるときの処理
		/// </summary>
		/// <param name="_pData">データのポインタ渡し</param>
		/// <returns>結果ハンドル</returns>
		HRESULT Close(LPCVOID _pData) override;
	};
}
