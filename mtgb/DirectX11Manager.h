#pragma once
#include "ISystem.h"
#include <string>

#pragma comment(lib, "d3d11.lib")  // DirectX11のライブラリ
#pragma comment(lib, "d3dcompiler.lib")  // シェーダコンパイラ用ライブラリ

struct D3D11_INPUT_ELEMENT_DESC;
struct CD3D11_RASTERIZER_DESC;
struct D3D11_VIEWPORT;
struct IDXGISwapChain1;
struct IDXGIOutput;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
class  WindowContext;
typedef struct HWND__* HWND;

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

		void InitializeCommonResources();
		void CreateDXGISurface(IDXGISwapChain1* pSwapChain1, IDXGISurface** ppDXGISurface);
		
		void CreateOutput(int index, IDXGIOutput** ppOutput);
		void CreateSwapChain(HWND hWnd, IDXGIOutput* pOutput, IDXGISwapChain1** ppSwapChain1);
		void CreateRenderTargetView(IDXGISwapChain1* pSwapChain1, ID3D11RenderTargetView** ppRenderTargetView);
		void CreateViewport(D3D11_VIEWPORT& viewport);
		void CreateDepthStencilAndDepthStencilView(ID3D11Texture2D** ppDepthStencil, ID3D11DepthStencilView** ppDepthStencilView);
		void InitializeWindowContext(WindowContext& context, bool isMultiMonitor);

		void ChangeRenderContext(WindowContext& context);
		void ChangeViewport(const D3D11_VIEWPORT& viewport);
		void ChangeRenderTargets(ID3D11RenderTargetView* pRenderTargetView, ID3D11DepthStencilView* pDepthStencilView);
		void ChangeSwapChain(IDXGISwapChain1* pSwapChain1);
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
