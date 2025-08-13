#include "Ground.h"
#include "MTAssert.h"
#include "CameraSystem.h"

namespace
{
	// 簡易的ライトの向き
	const mtgb::Vector4 LIGHT_DIRECTION{ 0.0f, 0.0f, 1.0f, 0.0f };
	// 簡易的地形のワールド座標
	const mtgb::Vector3 WORLD_POSITION{ 0, -10, 0 };
}

mtgb::Ground::Ground(FbxNode* _pParent) :
	vertexCount_{ 0 },
	pVertexes_{ nullptr }
{
	if (_pParent != nullptr)
	{
		pNode_ = _pParent;
		pMesh_ = _pParent->GetMesh();
	}
}

mtgb::Ground::~Ground()
{
}

void mtgb::Ground::Initialize()
{
	vertexCount_ = pMesh_->GetControlPointsCount();
	polygonCount_ = pMesh_->GetPolygonCount();
	polygonVertexCount_ = pMesh_->GetPolygonVertexCount();
	IShader::Initialize();
}

void mtgb::Ground::Release()
{
}

void mtgb::Ground::Draw()
{
	using namespace DirectX;

	// カメラシステムへのアクセス用
	const CameraSystem& CAMERA{ Game::System<CameraSystem>() };

	IShader::Draw<ConstantBuffer, Vertex>(
		[&, this](ConstantBuffer* _pCB)
		{
			// TODO: 地形位置を別クラスから操作
			Matrix4x4 mWorld{ XMMatrixTranslation(WORLD_POSITION.x, WORLD_POSITION.y, WORLD_POSITION.z) };
			//_transform.GenerateWorldMatrix(&mWorld);

			Matrix4x4 mView{};  // ビュー行列
			CAMERA.GetViewMatrix(&mView);

			Matrix4x4 mProj{};  // プロジェクション行列
			CAMERA.GetProjMatrix(&mProj);

			_pCB->g_matrixWorldViewProj = XMMatrixTranspose(mWorld * mView * mProj);
			_pCB->g_matrixWorld = XMMatrixTranspose(mWorld);

			CAMERA.GetPosition(&_pCB->g_cameraPosition);
			// TODO: ライトの向きはLightSystemから操作
			_pCB->g_lightDirection = LIGHT_DIRECTION; // ライトの向き
			_pCB->g_isTexture = (false);

		},
		[this](ID3D11DeviceContext* _pDC)
		{

		},
		GetIndexCount());
}

void mtgb::Ground::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	// xyzアクセス用インデックス
	static enum { X, Y, Z };

	pVertexes_ = new Vertex[vertexCount_]{};

	for (uint32_t poly = 0; poly < polygonCount_; poly++)
	{
		for (uint32_t vertex = 0; vertex < GetVertexCount(); vertex++)
		{
			int index{ pMesh_->GetPolygonVertex(poly, vertex) };

			if (index < 0 || index >= static_cast<int>(vertexCount_))
			{
				massert(false && "頂点インデックスが範囲外 @Ground::InitializeVertexBuffer");
				continue;
			}

			// 頂点の座標
			FbxVector4 position{ pMesh_->GetControlPointAt(index) };
			pVertexes_[index].position =
			{
				static_cast<float>(position[X]),
				static_cast<float>(position[Y]),
				static_cast<float>(position[Z]),
			};

			// 頂点の法線
			FbxVector4 normal{};
			pMesh_->GetPolygonVertexNormal(poly, vertex, normal);
			pVertexes_[index].normal =
			{
				static_cast<float>(normal[X]),
				static_cast<float>(normal[Y]),
				-static_cast<float>(normal[Z]),
			};
		}
	}

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(Vertex) * vertexCount_,
		.Usage = D3D11_USAGE_DYNAMIC,                // 途中で書き換えるため dynamic
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,    // 途中で書き換えるから
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = pVertexes_,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pVertexBuffer_);

	massert(SUCCEEDED(hResult)
		&& "頂点バッファの作成に失敗 @FbxParts::InitializeVertexBuffer");
}

void mtgb::Ground::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	pIndexData_ = new DWORD[GetIndexCount()]{};

	// ポリゴンを見ていく
	for (uint32_t poly = 0; poly < polygonCount_; poly++)
	{
		int polygonSize{ pMesh_->GetPolygonSize(poly) };
		massert(polygonSize == 3 && "Fbxモデルが三角化されていない @Ground::InitializeIndexBuffer");

		// 頂点を見ていく
		for (int v = 0; v < polygonSize; v++)
		{
			int index{ pMesh_->GetPolygonVertex(poly, v) };
			pIndexData_[GetVertexCount(poly) + v] = static_cast<DWORD>(index);
		}
	}

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(pIndexData_),
		.Usage = D3D11_USAGE_DEFAULT,          // 途中で書き換えない
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = pIndexData_,
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

void mtgb::Ground::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(ConstantBuffer),
		.Usage = D3D11_USAGE_DYNAMIC,  // MEMO: 途中で書き換えるためdynamic
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		nullptr,  // 初期データなし
		&pConstantBuffer_);

	massert(SUCCEEDED(hResult)
		&& "コンスタントバッファの作成に失敗 @FbxParts::InitializeConstantBuffer");
}
