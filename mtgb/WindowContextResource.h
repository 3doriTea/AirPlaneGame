#pragma once
#include <tuple>
#include <map>
#include <typeindex>
#include <d3d11.h>
#include "WindowContext.h"

struct IDXGISwapChain1;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID2D1RenderTarget;

namespace mtgb
{
	/// <summary>
	/// ウィンドウごとに固有のリソースのインターフェース
	/// </summary>
	class WindowContextResource
	{
	public:
		virtual ~WindowContextResource() {};
		virtual void Initialize(WindowContext _windowContext) = 0;
		/// <summary>
		/// ウィンドウを変更する際に固有のリソースをセットする
		/// </summary>
		virtual void SetResource() = 0;

		/// <summary>
		/// 更新処理
		/// </summary>
		virtual void Update() {};
		/// <summary>
		/// リソースのコピーを作成、返す
		/// </summary>
		/// <returns>生成したオブジェクト</returns>
		virtual WindowContextResource* Clone() const = 0;
	};
}