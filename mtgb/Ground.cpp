#include "Ground.h"
#include "MTAssert.h"
#include "CameraSystem.h"

namespace
{
	// �ȈՓI���C�g�̌���
	const mtgb::Vector4 LIGHT_DIRECTION{ 0.0f, 0.0f, 1.0f, 0.0f };
	// �ȈՓI�n�`�̃��[���h���W
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

	// �J�����V�X�e���ւ̃A�N�Z�X�p
	const CameraSystem& CAMERA{ Game::System<CameraSystem>() };

	IShader::Draw<ConstantBuffer, Vertex>(
		[&, this](ConstantBuffer* _pCB)
		{
			// TODO: �n�`�ʒu��ʃN���X���瑀��
			Matrix4x4 mWorld{ XMMatrixTranslation(WORLD_POSITION.x, WORLD_POSITION.y, WORLD_POSITION.z) };
			//_transform.GenerateWorldMatrix(&mWorld);

			Matrix4x4 mView{};  // �r���[�s��
			CAMERA.GetViewMatrix(&mView);

			Matrix4x4 mProj{};  // �v���W�F�N�V�����s��
			CAMERA.GetProjMatrix(&mProj);

			_pCB->g_matrixWorldViewProj = XMMatrixTranspose(mWorld * mView * mProj);
			_pCB->g_matrixWorld = XMMatrixTranspose(mWorld);

			CAMERA.GetPosition(&_pCB->g_cameraPosition);
			// TODO: ���C�g�̌�����LightSystem���瑀��
			_pCB->g_lightDirection = LIGHT_DIRECTION; // ���C�g�̌���
			_pCB->g_isTexture = (false);

		},
		[this](ID3D11DeviceContext* _pDC)
		{

		},
		GetIndexCount());
}

void mtgb::Ground::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	// xyz�A�N�Z�X�p�C���f�b�N�X
	static enum { X, Y, Z };

	pVertexes_ = new Vertex[vertexCount_]{};

	for (uint32_t poly = 0; poly < polygonCount_; poly++)
	{
		for (uint32_t vertex = 0; vertex < GetVertexCount(); vertex++)
		{
			int index{ pMesh_->GetPolygonVertex(poly, vertex) };

			if (index < 0 || index >= static_cast<int>(vertexCount_))
			{
				massert(false && "���_�C���f�b�N�X���͈͊O @Ground::InitializeVertexBuffer");
				continue;
			}

			// ���_�̍��W
			FbxVector4 position{ pMesh_->GetControlPointAt(index) };
			pVertexes_[index].position =
			{
				static_cast<float>(position[X]),
				static_cast<float>(position[Y]),
				static_cast<float>(position[Z]),
			};

			// ���_�̖@��
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
		.Usage = D3D11_USAGE_DYNAMIC,                // �r���ŏ��������邽�� dynamic
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,    // �r���ŏ��������邩��
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
		&& "���_�o�b�t�@�̍쐬�Ɏ��s @FbxParts::InitializeVertexBuffer");
}

void mtgb::Ground::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	pIndexData_ = new DWORD[GetIndexCount()]{};

	// �|���S�������Ă���
	for (uint32_t poly = 0; poly < polygonCount_; poly++)
	{
		int polygonSize{ pMesh_->GetPolygonSize(poly) };
		massert(polygonSize == 3 && "Fbx���f�����O�p������Ă��Ȃ� @Ground::InitializeIndexBuffer");

		// ���_�����Ă���
		for (int v = 0; v < polygonSize; v++)
		{
			int index{ pMesh_->GetPolygonVertex(poly, v) };
			pIndexData_[GetVertexCount(poly) + v] = static_cast<DWORD>(index);
		}
	}

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(pIndexData_),
		.Usage = D3D11_USAGE_DEFAULT,          // �r���ŏ��������Ȃ�
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

	massert(SUCCEEDED(hResult)  // �C���f�b�N�X�o�b�t�@�̍쐬�ɐ���
		&& "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s @Figure::InitializeIndexBuffer");
}

void mtgb::Ground::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(ConstantBuffer),
		.Usage = D3D11_USAGE_DYNAMIC,  // MEMO: �r���ŏ��������邽��dynamic
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		nullptr,  // �����f�[�^�Ȃ�
		&pConstantBuffer_);

	massert(SUCCEEDED(hResult)
		&& "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s @FbxParts::InitializeConstantBuffer");
}
