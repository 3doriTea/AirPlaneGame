#pragma once
#include <functional>
#include <d3d11.h>
#include "DirectX11Draw.h"
#include "Screen.h"
#include "Transform.h"


struct ID3D11Buffer;

namespace mtgb
{
	class IShader
	{
	public:
		IShader();
		virtual ~IShader()
		{}

		/// <summary>
		/// シェーダの初期化処理
		/// </summary>
		virtual void Initialize();
		//virtual void Draw() = 0;

	protected:
		/// <summary>
		/// 頂点バッファの初期化
		/// </summary>
		virtual void InitializeVertexBuffer(ID3D11Device* _pDevice) = 0;
		/// <summary>
		/// インデックスバッファの初期化
		/// </summary>
		virtual void InitializeIndexBuffer(ID3D11Device* _pDevice) = 0;
		/// <summary>
		/// コンスタントバッファの初期化
		/// </summary>
		virtual void InitializeConstantBuffer(ID3D11Device* _pDevice) = 0;

		/// <summary>
		/// シェーダの描画処理(共通)
		/// </summary>
		/// <typeparam name="ConstantBufferT">コンスタントバッファの型</typeparam>
		/// <typeparam name="VertexT">頂点の型</typeparam>
		/// <param name="_makeConstantBufferCallback">
		/// <para>コンスタントバッファに入れる用のコールバック関数</para>
		/// <para>コールバック関数:(ConstantBufferT* _pCB)->void</para>
		/// </param>
		/// <param name="_contextSetterCallback">
		/// <para>コンスタントバッファ送信後のコンテキスト処理のコールバック関数</para>
		/// <para>コールバック関数:(ID3D11DeviceContext*)->void</para>
		/// </param>
		template<typename ConstantBufferT, typename VertexT>
		void Draw(
			const std::function<void(ConstantBufferT* _pConstantBuffer)>& _makeConstantBufferCallback,
			const std::function<void(ID3D11DeviceContext* _pContext)>& _contextSetterCallback);

		/// <summary>
		/// カメラの座標系を取得
		/// </summary>
		/// <returns>カメラTransfromの参照ポインタ</returns>
		const mtgb::Transform& GetCameraTransfrom();

	protected:
		ID3D11Buffer* pVertexBuffer_;    // 頂点の位置、色
		ID3D11Buffer* pIndexBuffer_;     // ポリゴンを結ぶ順番
		ID3D11Buffer* pConstantBuffer_;  //　シェーダのグローバル変数
	};

	template<typename ConstantBufferT, typename VertexT>
	inline void IShader::Draw(
		const std::function<void(ConstantBufferT*)>& _makeConstantBufferCallback,
		const std::function<void(ID3D11DeviceContext*)>& _contextSetterCallback)
	{
		UINT stride{ 0U };
		UINT offset{ 0U };

		stride = sizeof(VertexT);
		offset = 0;
		DirectX11Draw::pContext_->IASetVertexBuffers(0U, 1U, &pVertexBuffer_, &stride, &offset);  // 頂点バッファをセット
		DirectX11Draw::pContext_->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);  // インデックスバッファをセット
		DirectX11Draw::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);  // 頂点シェーダのコンスタントバッファをセット
		DirectX11Draw::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);  // ピクセルシェーダのコンスタントバッファをセット

		ConstantBufferT constantBuffer{};

		_makeConstantBufferCallback(&constantBuffer);

		HRESULT hResult{};

		// シェーダに渡すためのデータ
		D3D11_MAPPED_SUBRESOURCE mappedSubresource{};

		// GPUからのデータアクセスをせき止める
		hResult = DirectX11Draw::pContext_->Map(
			pConstantBuffer_,
			NULL,
			D3D11_MAP_WRITE_DISCARD,
			NULL,
			&mappedSubresource);

		massert(SUCCEEDED(hResult)  // GPUデータアクセスせき止めに成功
			&& "GPUデータアクセスせき止めに失敗");

		// データ書き込み
		memcpy_s(
			mappedSubresource.pData,
			mappedSubresource.RowPitch,
			static_cast<void*>(&constantBuffer),
			sizeof(ConstantBufferT));

		// GPUデータアクセスせき止め解除
		DirectX11Draw::pContext_->Unmap(pConstantBuffer_, 0);

		_contextSetterCallback(DirectX11Draw::pContext_);

		DirectX11Draw::pContext_->DrawIndexed(6, 0, 0);
	}
}
