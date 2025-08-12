#include "Figure.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "DirectX11Draw.h"
#include "RectInt.h"
#include "Game.h"
#include "Screen.h"


mtgb::Figure::Figure()
{
}

mtgb::Figure::~Figure()
{
}

void mtgb::Figure::Draw(
	const RectInt& _rect,
	const Color& _color)
{
	using DirectX::XMMatrixScaling;      // 拡縮
	using DirectX::XMMatrixTranspose;    // 行と列を入れ替える
	using DirectX::XMMatrixTranslation;  // 平行移動
	using DirectX::XMMatrixIdentity;     // 単位行列

	DirectX11Draw::SetBlendMode(BlendMode::Default);  // ブレンドモードデフォルト
	DirectX11Draw::SetIsWriteToDepthBuffer(false);    // 深度バッファへの書き込みなし

	IShader::Draw<ConstantBuffer, Vertex>(
		[&, this](ConstantBuffer* _pCB)
		{
			_pCB->g_color = _color.ToVector4Norm();
			_pCB->g_worldMatrix = XMMatrixIdentity();
#pragma region TODO: 計算見直し必要
			// スクリーンサイズを一度だけ取得
			static const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

			// 数学座標と描画座標のy軸差異解消
			RectInt cartesianBox{ _rect };
			cartesianBox.y = SCREEN_SIZE.y - cartesianBox.y;
			cartesianBox.height *= -1;

			const Vector2Int VIEW_BEGIN{ cartesianBox.GetBegin() };
			const Vector2Int VIEW_END{ cartesianBox.GetEnd() };

			// 表示するサイズに合わせる
			Matrix4x4 scalingBox = XMMatrixScaling(
				std::abs(VIEW_END.x - VIEW_BEGIN.x) * 2.0f,
				std::abs(VIEW_END.y - VIEW_BEGIN.y) * 2.0f,
				1.0f);

			// 表示するボックスの位置を移動する
			Matrix4x4 moveBox = XMMatrixTranslation(
				((VIEW_END.x - VIEW_BEGIN.x) / 2.0f + VIEW_BEGIN.x) / (SCREEN_SIZE.x / 2.0f),
				((VIEW_BEGIN.y - VIEW_END.y) / 2.0f + VIEW_END.y) / (SCREEN_SIZE.y / 2.0f),
				0.0f);

			// 画面に合わせる
			Matrix4x4 scalingView = XMMatrixScaling(
				1.0f / (SCREEN_SIZE.x * 2),
				1.0f / (SCREEN_SIZE.y * 2),
				1.0f);

			// オフセット - 画面中心は(0, 0) 左下は(-1, -1)
			Matrix4x4 offsetView
			{
				XMMatrixTranslation(-1.0f, -1.0f, 0.0f)
			};

			// 最終的な行列
			Matrix4x4 world
			{
				scalingBox * scalingView * moveBox * offsetView
			};
#pragma endregion

			// MEMO: CPU -> 行優先, GPU -> 列優先
			//  REF: https://qiita.com/niusounds/items/65099654673f5df3be9b
			_pCB->g_worldMatrix = XMMatrixTranspose(world);
		},
		[&, this](ID3D11DeviceContext* _pDC)
		{
		});
}

void mtgb::Figure::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	Vertex vertexes[]
	{
		Vector3{ -1,  1,  0 },  // 左上
		Vector3{  1,  1,  0 },  // 右上
		Vector3{ -1, -1,  0 },  // 左下
		Vector3{  1, -1,  0 },  // 右下
	};

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(vertexes),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = vertexes,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pVertexBuffer_);

	massert(SUCCEEDED(hResult)  // 頂点バッファの作成に成功
		&& "頂点バッファの作成に失敗 @Figure::InitializeVertexBuffer");
}

void mtgb::Figure::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	static const int INDEXES[]{ 2, 1, 0, 2, 3, 1 };

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(INDEXES),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = INDEXES,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pIndexBuffer_);

	massert(SUCCEEDED(hResult)  // インデックスバッファの作成に成功
		&& "インデックスバッファの作成に失敗 @Figure::InitializeIndexBuffer");
}

void mtgb::Figure::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	static const ConstantBuffer INITIALIZE_CONSTANT_BUFFER
	{
		.g_color = { 1, 0, 0, 1 },
		.g_worldMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()),
	};

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(ConstantBuffer),
		.Usage = D3D11_USAGE_DYNAMIC,  // MEMO: 途中で書き換えるためdynamic
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = &INITIALIZE_CONSTANT_BUFFER,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pConstantBuffer_);

	massert(SUCCEEDED(hResult)  // コンスタントバッファの作成に成功
		&& "コンスタントバッファの作成に失敗 @Figure::InitializeConstantBuffer");
}
