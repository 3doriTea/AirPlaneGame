#include "FbxParts.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "ReleaseUtility.h"
#include "MTAssert.h"
#include "Transform.h"
#include "Debug.h"
#include "MTStringUtility.h"

mtgb::FbxParts::FbxParts(FbxNode* parent): pMesh_(nullptr)
{
	if (parent != nullptr)
	{
		pMesh_ = parent->GetMesh();
	}

	massert(pMesh_ != nullptr && "FbxParts: pMesh_ is null");
}

mtgb::FbxParts::~FbxParts()
{
}

void mtgb::FbxParts::Initialize()
{
	IShader::Initialize();
}

void mtgb::FbxParts::Draw(const Transform& _transfrom)
{
	// �`������V�F�[�_�ɓn��
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	//DirectX11Draw::
}

void mtgb::FbxParts::DrawSkinAnimation(const Transform& _transform, FbxTime _time)
{
}

void mtgb::FbxParts::DrawSkinAnimation(const std::string& _takeName, const Transform& _transform, FbxTime _time)
{
	using namespace DirectX;

	for (int i = 0; i < boneCount_; i++)
	{
		FbxAnimEvaluator* evaluator{ ppCluster_[i]->GetLink()->GetScene()->GetAnimationEvaluator() };
		FbxMatrix mCurrent{ evaluator->GetNodeGlobalTransform(ppCluster_[i]->GetLink(), _time) };

		// Fbx to DirectX matrix
		XMFLOAT4X4 pose{};
		for (DWORD x = 0; x < 4; x++)
		{
			for (DWORD y = 0; y < 4; y++)
			{
				pose(x, y) = static_cast<float>(mCurrent.Get(x, y));
			}
		}

		XMFLOAT4X4 mat{};
		XMMATRIX mMirror{ XMMatrixIdentity() };
		XMStoreFloat4x4(&mat, mMirror);
		mat.m[2][2] = -1.0f;

		mMirror = XMLoadFloat4x4(&mat);

		pBones_[i].newPose = XMLoadFloat4x4(&pose) * mMirror;
		pBones_[i].diffPose = XMMatrixInverse(nullptr, pBones_[i].bindPose * mMirror);
		pBones_[i].diffPose = pBones_[i].diffPose * pBones_[i].newPose;
	}

	for (DWORD i = 0; i < vertexCount_; i++)
	{
		Matrix4x4 matrix{};
		for (int m = 0; m < boneCount_; m++)
		{
			if (pWeights_[i].pBoneIndex[m] < 0)
			{
				break;
			}
			matrix += pBones_[pWeights_[i].pBoneIndex[m]].diffPose * pWeights_[i].pBoneWeight[m];
		}

		XMVECTOR position{ XMLoadFloat3(&pWeights_[i].posOrigin) };
		XMVECTOR normal{ XMLoadFloat3(&pWeights_[i].normalOrigin) };

		XMStoreFloat3(&pVertexes_[i].position, XMVector3TransformCoord(position, matrix));
		XMFLOAT3X3 mat{};
		XMStoreFloat3x3(&mat, matrix);
		matrix = XMLoadFloat3x3(&mat);
		XMStoreFloat3(&pVertexes_[i].normal, XMVector3TransformCoord(normal, matrix));
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubResource{};
	//DirectX11Draw::
}

void mtgb::FbxParts::DrawMeshAnimation(const Transform& _transform, FbxTime _time)
{
	Draw(_transform);
}

bool mtgb::FbxParts::TryGetBonePosition(const std::string& _boneName, Vector3* _pPosition)
{
	for (int i = 0; i < boneCount_; i++)
	{
		// ���������I
		if (_boneName == ppCluster_[i]->GetLink()->GetName())
		{
			FbxAMatrix m{};
			ppCluster_[i]->GetTransformLinkMatrix(m);

			_pPosition->x = static_cast<float>(m[3][0]);
			_pPosition->y = static_cast<float>(m[3][1]);
			_pPosition->z = static_cast<float>(m[3][2]);

			return true;
		}
	}

	return false;
}

bool mtgb::FbxParts::TryGetBonePositionAtNow(const std::string& _boneName, Vector3* _pPosition)
{
	auto itr{ boneNamePair_.find(_boneName) };
	if (itr != boneNamePair_.end())  // end ����Ȃ��Ȃ猩������
	{
		Matrix4x4 m{};
		//DirectX::XMStoreFloat4x4(&m, itr->second->newPose);
		_pPosition->x = m.r[3].m128_f32[0];
		_pPosition->y = m.r[3].m128_f32[1];
		_pPosition->z = m.r[3].m128_f32[2];

		return true;
	}
	return false;
}

void mtgb::FbxParts::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	pVertexes_ = new Vertex[vertexCount_]{};

	for (uint32_t poly = 0; poly < polygonCount_; poly++)
	{
		for (uint32_t vertex = 0; vertex < 3; vertex++)
		{
			int index{ pMesh_->GetPolygonVertex(poly, vertex) };

			// ���_�̍��W
			FbxVector4 position = pMesh_->GetControlPointAt(index);
			pVertexes_[index].position =
			{
				static_cast<float>(position[0]),
				static_cast<float>(position[1]),
				static_cast<float>(position[2]),
			};

			// ���_�̖@��
			FbxVector4 normal;
			pMesh_->GetPolygonVertexNormal(poly, vertex, normal);
		}
	}

	int uvCount{ pMesh_->GetTextureUVCount() };
	FbxLayerElementUV* pUV{ pMesh_->GetLayer(0)->GetUVs() };
	if (uvCount > 0 && pUV->GetMappingMode() == FbxLayerElement::eByControlPoint)
	{
		for (int i = 0; i < uvCount; i++)
		{
			FbxVector2 uv{ pUV->GetDirectArray().GetAt(i) };
			pVertexes_[i].uv =
			{
				static_cast<float>(uv.mData[0]),
				1.0f - static_cast<float>(uv.mData[1]),
				0.0f
			};
		}
	}

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(Vertex) * pMesh_->GetControlPointsCount(),
		.Usage = D3D11_USAGE_DYNAMIC,  // MEMO: �r���ŏ��������邽�� dynamic
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,  // �r���ŏ��������邩��
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = pVertexes_,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	_pDevice->CreateBuffer(&BUFFER_DESC, &INITIALIZE_DATA, &pVertexBuffer_);
}

void mtgb::FbxParts::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	ID3D11Buffer** ppIndexBuffer{ new ID3D11Buffer* [materialCount_] };
	DWORD** ppIndexData{ new DWORD* [materialCount_] };

	int count{ 0 };
	for (DWORD i = 0; i < materialCount_; i++)
	{
		count = 0;
		DWORD* pIndex{ new DWORD[polygonCount_ * 3]{} };
		for (DWORD j = 0; j < polygonCount_; j++)
		{
			FbxLayerElementMaterial* material{ pMesh_->GetLayer(0)->GetMaterials() };
			int materialId{ material->GetIndexArray().GetAt(j) };
			if (materialId == i)
			{
				for (DWORD k = 0; k < 3; k++)
				{
					pIndex[count + k] = pMesh_->GetPolygonVertex(j, k);
				}
				count += 3;
			}
		}

		// �o�b�t�@�̐ݒ�
		const D3D11_BUFFER_DESC BUFFER_DESC
		{
			.ByteWidth = sizeof(int) * count,
			.Usage = D3D11_USAGE_DEFAULT,  // MEMO: �r���ŏ��������Ȃ����� DEFAULT
			.BindFlags = D3D11_BIND_INDEX_BUFFER,
			.CPUAccessFlags = 0,
			.MiscFlags = 0,
			.StructureByteStride = 0,
		};

		// �����f�[�^
		const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
		{
			.pSysMem = pIndex,
			.SysMemPitch = 0,
			.SysMemSlicePitch = 0,
		};

		HRESULT hResult{};
		hResult = _pDevice->CreateBuffer(&BUFFER_DESC, &INITIALIZE_DATA, &ppIndexBuffer[i]);

		massert(SUCCEEDED(hResult)
			&& "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s");

		pMaterial_[i].polygonCount = count / 3;
		ppIndexData[i] = new DWORD[count];
		memcpy(ppIndexData[i], pIndex, sizeof(DWORD) * count);
		SAFE_DELETE_ARRAY(pIndex);
	}
}

void mtgb::FbxParts::InitializeConstantBuffer(ID3D11Device* _pDevice)
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

void mtgb::FbxParts::InitializeMaterial()
{
	// �}�e���A���o�b�t�@�̍쐬
	materialCount_ = pNode_->GetMaterialCount();
	pMaterial_ = new Material[materialCount_]{};

	for (DWORD i = 0; i < materialCount_; i++)
	{
		FbxSurfaceMaterial* pSurfaceMaterial = pNode_->GetMaterial(i);

		FbxSurfacePhong* pPhong = static_cast<FbxSurfacePhong*>(pSurfaceMaterial);

		// ���� 
		FbxDouble3 ambient = FbxDouble3(0, 0, 0);
		// �g�U���ˌ�
		FbxDouble3 diffuse = FbxDouble3(0, 0, 0);
		// ���ʔ���
		FbxDouble3 specular = FbxDouble3(0, 0, 0);

		// Ambient�̃v���p�e�B��������
		FbxProperty prop{};

		prop = pPhong->FindProperty(FbxSurfaceMaterial::sAmbient);
		if (prop.IsValid())
		{
			ambient = pPhong->Ambient;
		}
		prop = pPhong->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if (prop.IsValid())
		{
			diffuse = pPhong->Diffuse;
		}
		// MEMO: ������typedef double FbxDouble ����Ă��邽��float�ɃL���X�g
		pMaterial_[i].specular = { (float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f };
		pMaterial_[i].diffuse = { (float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f };
		pMaterial_[i].specular = { 0, 0, 0, 0 };
		pMaterial_[i].shininess = 0;

		if (pSurfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			prop = pPhong->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (prop.IsValid())  // �L���l�Ȃ�
			{
				specular = pPhong->Specular;
			}

			pMaterial_[i].specular = { (float)specular[0], (float)specular[1], (float)specular[2], 1.0f };
			prop = pPhong->FindProperty(FbxSurfaceMaterial::sShininess);

			if (prop.IsValid())
			{
				pMaterial_[i].shininess = (float)pPhong->Shininess;
			}
			else  // Shininess�̒l�������Ȃ�
			{
				pMaterial_[i].shininess = 1.0f;
			}
		}
		InitializeTexture(pSurfaceMaterial, i);
	}
}

void mtgb::FbxParts::InitializeTexture(FbxSurfaceMaterial* _pMaterial, const DWORD _i)
{
	// �e�N�X�`���̏��
	FbxProperty prop = _pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

	// �e�N�X�`����
	int fileTextureCount{ prop.GetSrcObjectCount<FbxFileTexture>() };

	if (fileTextureCount > 0)  // �e�N�X�`��������Ȃ�
	{
		FbxFileTexture* pTexture{ prop.GetSrcObject<FbxFileTexture>() };

		// �t�@�C���� + �g�� �����ɂ���
		char name[_MAX_FNAME]{};
		char ext[_MAX_EXT]{};
		_splitpath_s(pTexture->GetRelativeFileName(), nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);
		wsprintf(name, "%s%s", name, ext);

		pMaterial_[_i].pTexture = new Texture2D{};
		pMaterial_[_i].pTexture->Load(ToWString(std::string{ name }));
	}
}

void mtgb::FbxParts::InitializeSkelton()
{
	FbxDeformer* pDeformer{ pMesh_->GetDeformer(0) };
	if (pDeformer == nullptr)
	{
		return;  // �{�[�����Ȃ����߉�A
	}

	pSkin_ = static_cast<FbxSkin*>(pDeformer);

	struct POLY_INDEX
	{
		int* polyIndex;    // �|���S���̔ԍ�
		int* vertexIndex;  // ���_�̔ԍ�
		int numRef;        // ���_�����L����|���S����
	};

	// �e�{�[���̏����擾����
	boneCount_ = pSkin_->GetClusterCount();
	ppCluster_ = new FbxCluster* [boneCount_];
	for (int i = 0; i < boneCount_; i++)
	{
		ppCluster_[i] = pSkin_->GetCluster(i);
	}

	// �{�[���̐��ɍ��킹�ăE�F�C�g��������
	pWeights_ = new Weight[vertexCount_];
	for (DWORD i = 0; i < vertexCount_; i++)
	{
		pWeights_[i].posOrigin = pVertexes_[i].position;
		pWeights_[i].normalOrigin = pVertexes_[i].normal;
		pWeights_[i].pBoneIndex = new int[boneCount_];
		pWeights_[i].pBoneWeight = new float[boneCount_];
		for (int j = 0; j < boneCount_; j++)
		{
			pWeights_[i].pBoneIndex[j] = -1;
			pWeights_[i].pBoneWeight[j] = 0.0f;
		}
	}

	for (int i = 0; i < boneCount_; i++)
	{
		// �{�[���ɉe����^���钸�_��
		int indicesCount{ ppCluster_[i]->GetControlPointIndicesCount() };
		// �{�[���E�E�F�C�g�̏��ԍ�
		int* pIndex{ ppCluster_[i]->GetControlPointIndices() };
		// ���_���Ƃ̃E�F�C�g���
		double* pWeight{ ppCluster_[i]->GetControlPointWeights() };

		for (int k = 0; k < indicesCount; k++)
		{
			for (int m = 0; m < 4; m++)
			{
				if (m >= boneCount_)
				{
					break;
				}

				if (pWeight[k] > pWeights_[pIndex[k]].pBoneWeight[m])
				{
					for (int n = boneCount_ - 1; n > m; n--)
					{
						pWeights_[pIndex[k]].pBoneIndex[n] = pWeights_[pIndex[k]].pBoneIndex[n - 1];
						pWeights_[pIndex[k]].pBoneWeight[n] = pWeights_[pIndex[k]].pBoneWeight[n - 1];
					}
					
					pWeights_[pIndex[k]].pBoneIndex[m] = i;
					pWeights_[pIndex[k]].pBoneWeight[m] = (float)pWeight[k];
					break;
				}
			}
		}
	}

	// �{�[�����
	pBones_ = new Bone[boneCount_];
	for (int i = 0; i < boneCount_; i++)
	{
		FbxAMatrix matrix;
		ppCluster_[i]->GetTransformLinkMatrix(matrix);

		DirectX::XMFLOAT4X4 pose{};
		for (DWORD x = 0; x < 4; x++)
		{
			for (DWORD y = 0; y < 4; y++)
			{
				pose(x, y) = (float)matrix.Get(x, y);
			}
		}
		pBones_[i].bindPose = DirectX::XMLoadFloat4x4(&pose);
		boneNamePair_[ppCluster_[i]->GetLink()->GetName()] = pBones_ + i;
	}
}
