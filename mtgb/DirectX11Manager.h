#pragma once
#include "ISystem.h"
#include <string>
#include <wrl/client.h>
#include "Vector2Int.h"
#include <dxgi1_2.h> 
#include <vector>
#include <optional>
#include "MonitorInfo.h"

#pragma comment(lib, "d3d11.lib")  // DirectX11のライブラリ
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")  // シェーダコンパイラ用ライブラリ

using Microsoft::WRL::ComPtr;
struct D3D11_INPUT_ELEMENT_DESC;
struct CD3D11_RASTERIZER_DESC;
struct D3D11_VIEWPORT;
struct IDXGISwapChain1;
struct IDXGISurface;
struct IDXGIOutput;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
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
		
		//void CreateOutput(int index, IDXGIOutput** ppOutput);
		void CreateSwapChain(HWND hWnd, IDXGIOutput* pOutput, IDXGISwapChain1** ppSwapChain1);
		void CreateRenderTargetView(IDXGISwapChain1* pSwapChain1, ID3D11RenderTargetView** ppRenderTargetView);
		void CreateViewport(const Vector2Int& size, D3D11_VIEWPORT& viewport);
		void CreateDepthStencilAndDepthStencilView(const Vector2Int bufSize, ID3D11Texture2D** ppDepthStencil, ID3D11DepthStencilView** ppDepthStencilView);


		void ChangeViewport(const D3D11_VIEWPORT& viewport);
		void ChangeRenderTargets(ComPtr<ID3D11RenderTargetView> pRenderTargetView, ComPtr<ID3D11DepthStencilView> pDepthStencilView);
		void ChangeSwapChain(ComPtr<IDXGISwapChain1> pSwapChain1);
				
		void ClearState();
		/// <summary>
		/// ClearState後に必要なデフォルト状態を再設定
		/// </summary>
		void SetDefaultStates();
		
		/// <summary>
		/// 利用可能なモニターを自動で割り当てる
		/// </summary>
		/// <param name="ppOutput">これにモニターを割り当てる</param>
		/// <returns>割り当てられたモニターのインデックス、失敗時は -1</returns>
		std::optional<MonitorInfo> AssignAvailableMonitor(IDXGIOutput** ppOutput);

		int GetAvailableMonitorCount() const;

		void Release() override;
	private:
		std::vector<DXGI_ADAPTER_DESC1> adaptersDesc_;
		/// <summary>
		/// 利用可能なモニターを列挙する
		/// </summary>
		void EnumAvailableMonitors();
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
