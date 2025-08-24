#pragma once
#include <string_view>
#include <d3d11.h>
#include <array>
#include <functional>


namespace mtgb
{
	/*
	* TODO:
		・シェーダパス
		・識別列挙子
		・D3D11_RASTERIZER_DESC
		・初期値：VertexBuffer
		・初期値：IndexBuffer
		・初期値：ConstantBuffer
		・GPUせき止め：VertexBuffer
		・GPUせき止め：IndexBuffer
		・GPUせき止め：ConstantBuffer
		・サンプラ用(ID3D11DeviceContext*)
		
		private:
		bool isRequired;  // 必要かのフラグ
	*/

	template<typename T>
	concept IsEnumStruct = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

	/// <summary>
	/// <para>mtgbで提供しないゲーム側のシェーダ</para>
	/// <para>これを継承して登録してください。</para>
	/// </summary>
	template<IsEnumStruct EnumStructT, typename OriginalShaderT>
	class IOriginalShader
	{
		//static_assert(IsEnumStruct, "スコープ付き列挙型を指定する必要があります。 @IOriginalShader");
	public:/*
		/// <summary>
		/// 頂点バッファを動的変更する場合継承する
		/// </summary>
		class IWriteVertex
		{
		public:
			IWriteVertex() {}
			virtual ~IWriteVertex() {}

			
		};
		class IWriteIndex
		{
		public:
			IWriteIndex() {}
			virtual ~IWriteIndex() {}

			
		};
		class IWriteConstant
		{
		public:
			IWriteConstant() {}
			virtual ~IWriteConstant() {}

			
		};*/

	public:
		IOriginalShader() {}
		virtual ~IOriginalShader() {}

	protected:
		virtual std::string_view GetFileName() = 0;

		/// <summary>
		/// 頂点バッファの初期化
		/// </summary>
		virtual void InitializeVertexBuffer(ID3D11Device* _pDevice);
		/// <summary>
		/// インデックスバッファの初期化
		/// </summary>
		virtual void InitializeIndexBuffer(ID3D11Device* _pDevice);
		/// <summary>
		/// コンスタントバッファの初期化
		/// </summary>
		virtual void InitializeConstantBuffer(ID3D11Device* _pDevice);

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="VertexT">頂点の型</typeparam>
		/// <typeparam name="VertexesSize"></typeparam>
		/// <param name="vertexes"></param>
		template<typename VertexT, size_t VertexesSize>
		void OnWriteVertexBuffer(std::function<void(std::array<VertexT, VertexesSize>&)> vertexes);

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="IndexSize"></typeparam>
		/// <param name="pIndexes"></param>
		template<size_t IndexSize>
		void OnWriteIndexBuffer(std::function<std::array<int, IndexSize>&> pIndexes);

		/// <summary>
		/// コンスタントバッファに書き込み
		/// </summary>
		/// <typeparam name="ConstantBufferT">コンスタントバッファの型</typeparam>
		/// <param name="_pConstantBuffer">コンスタントバッファのポインタ渡し</param>
		template<typename ConstantBufferT>
		void OnWriteConstantBuffer(std::function<ConstantBufferT*> _pConstantBuffer);

		void OnWriteOther(std::function<void(ID3D11DeviceContext*)> _pDC);

	protected:
		ID3D11Buffer* pVertexBuffer_;    // 頂点の位置、色
		ID3D11Buffer* pIndexBuffer_;     // ポリゴンを結ぶ順番
		ID3D11Buffer* pConstantBuffer_;  //　シェーダのグローバル変数

		int indexCount_;  // 描画するインデックス数
	private:
		bool isRequired_;  // override 検知用
	};
}
