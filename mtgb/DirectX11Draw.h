#pragma once
#include "cmtgb.h"
#include "Vector4.h"


struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGIDevice1;
struct IDXGIAdapter;
struct IDXGIFactory2;

struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct D3D11_VIEWPORT;
struct ID3D11BlendState;

struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;
struct ID3D11Texture2D;

namespace mtgb
{
	class Figure;
	class IShader;
	class MTImGui;
	class FbxParts;
	/// <summary>
	/// シェーダタイプ
	/// </summary>
	enum struct ShaderType : int8_t
	{
		Figure,
		Sprite2D,
		FbxParts,
		Max,
	};

	/// <summary>
	/// ブレンドモード
	/// </summary>
	enum struct BlendMode : int8_t
	{
		Default,

		Max,
	};

	/// <summary>
	/// GPUで行う処理のバンドル
	/// </summary>
	struct ShaderBundle
	{
		/// <summary>
		/// <para>頂点レイアウト</para>
		/// <para></para>
		/// </summary>
		ID3D11InputLayout* pVertexLayout;

		/// <summary>
		/// <para>頂点シェーダ</para>
		/// <para>頂点の情報</para>
		/// </summary>
		ID3D11VertexShader* pVertexShader;

		/// <summary>
		/// <para>ピクセルシェーダ</para>
		/// <para></para>
		/// </summary>
		ID3D11PixelShader* pPixelShader;

		/// <summary>
		/// <para>ラスタライザ</para>
		/// <para>どのピクセルを光らせるかの情報</para>
		/// </summary>
		ID3D11RasterizerState* pRasterizerState;
	};

	/// <summary>
	/// DirectX11で描画するやつ
	/// </summary>
	class DirectX11Draw final
	{
		friend class DirectX11Manager;
		friend class Texture2D;
		friend class IShader;
		friend class DirectWrite;
		friend class Direct2D;
		friend class OBJ;
		friend class MTImGui;
		friend class FbxParts;

	public:
		/// <summary>
		/// 描画するシェーダをセットする
		/// </summary>
		/// <param name="_type">シェーダの種類</param>
		static void SetShader(const ShaderType _type);
		/// <summary>
		/// 描画するブレンドモードをセットする
		/// </summary>
		/// <param name="_mode">ブレンドモード</param>
		static void SetBlendMode(const BlendMode _mode);
		/// <summary>
		/// 深度バッファへの書き込みをするか
		/// </summary>
		/// <param name="_enabled">書き込みをする true / false</param>
		static void SetIsWriteToDepthBuffer(const bool _enabled);

	private:
		DirectX11Draw() = delete;
		~DirectX11Draw() = delete;

		/// <summary>
		/// 描画開始 (キャンバスをきれいにする)
		/// </summary>
		static void Begin();

		/// <summary>
		/// 描画終了 (バックバッファとスワップする)
		/// </summary>
		static void End();

		/// <summary>
		/// 解放処理
		/// </summary>
		static void Release();

	private:
		static ID3D11Device* pDevice_;                                                             // 描画を行うための環境、リソースの作成に使う
		static ID3D11DeviceContext* pContext_;                                                     // GPUに命令出すやつ
		static IDXGIDevice1* pDXGIDevice_;
		static IDXGIAdapter* pDXGIAdapter_;
		static IDXGIFactory2* pDXGIFactory_;

		static IDXGISwapChain* pSwapChain_;                                                        // ダブルバッファリングするやつ
		static ID3D11RenderTargetView* pRenderTargetView_;                                         // 描画先
		static ID3D11DepthStencilView* pDepthStencilView_;                                         // 深度バッファ
		static ID3D11DepthStencilState* pDepthStencilState_[static_cast<int8_t>(BlendMode::Max)];  // ブレンドによる深度バッファへの書き込み情報
		static ID3D11Texture2D* pDepthStencil_;                                                    // 深度ステンシル
		static ID3D11BlendState* pBlendState_[static_cast<int8_t>(BlendMode::Max)];                // ブレンドの情報
		static ShaderBundle shaderBundle_[static_cast<int8_t>(ShaderType::Max)];                   // シェーダのバンドル
		static Vector4 backgroundColor_;
		static ID3D11SamplerState* pDefaultSamplerState_;
	};
}
