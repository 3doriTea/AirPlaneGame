#pragma once
#include "cmtgb.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "IShader.h"
#include "Texture2D.h"
#include <unordered_map>
#include <fbxsdk.h>


#pragma comment(lib, "LibFbxSDK-MT.lib")
#pragma comment(lib, "LibXml2-MT.lib")
#pragma comment(lib, "zlib-MT.lib")

namespace fbxsdk
{/*
	class FbxMesh;
	class FbxSkin;*/
	class FbxCluster;
}

namespace mtgb
{
	class Transform;

	class FbxParts : public IShader
	{
	public:
		struct Vertex
		{
			Vector3 position;  // ���W
			Vector3 normal;  // �@��
			Vector3 uv;  // uv���W
		};

		struct ConstantBuffer
		{
			Matrix4x4 g_matrixWorldViewProj;  // ���[���h�E�r���[�E�v���W�F�N�V�����s��
			Matrix4x4 g_matrixNormalTrans;    // ��]�s��Ɗg��s��̋t�s��
			Matrix4x4 g_matrixWorld;  // ���[���h�s��
			Vector4 g_lightDirection;  // ���C�g�̌���
			Vector4 g_diffuse;  // �������������Ƃ��ւ̊g�U���ˌ�(�}�e���A���F)
			Vector4 g_ambient;  // �S�̓I�Ȋ��� (����������Ȃ��ꏊ�ɂ����邭)
			Vector4 g_speculer;  // ���ʔ��� (Lambert�̏ꍇ��0)
			float g_shininess;     // �X�y�L�����̋���
			bool g_isTexture;  // �e�N�X�`���̗L��
		};

		/// <summary>
		/// �}�e���A��
		/// </summary>
		struct Material
		{
			uint32_t polygonCount;  // �|���S����
			Vector4 diffuse;  // �g�U���ˌ��ւ̔��ˋ��x
			Vector4 ambient;  // �����ւ̔��ˋ��x
			Vector4 specular;  // ���ʔ��ˌ�
			float shininess;  // �n�C���C�g�̋���
			Texture2D* pTexture;
		};

		/// <summary>
		/// �{�[�� (�֐߂��̂���)
		/// </summary>
		struct Bone
		{
			// REF: https://help.autodesk.com/view/MAYACRE/JPN/?guid=GUID-36808BCC-ACF9-4A9E-B0D8-B8F509FEC0D5
			Matrix4x4 bindPose;  // �����|�[�Y���̃{�[���ϊ��s��
			Matrix4x4 newPose;  // �A�j���[�V�����ŕω����̃{�[���ϊ��s��
			Matrix4x4 diffPose;  // bindPose�ɑ΂��� newPose�̕ω���
		};

		/// <summary>
		/// �E�F�C�g (�֐ߓ��m�̉e���x����)
		/// </summary>
		struct Weight
		{
			Vector3 posOrigin;  // ���Ƃ��Ƃ̒��_���W
			Vector3 normalOrigin;  // ���Ƃ��Ƃ̖@��
			int* pBoneIndex;  // �֘A����{�[����Id
			float* pBoneWeight;  // �{�[���̏d��
		};

	public:
		FbxParts();
		~FbxParts();

		void Initialize() override;

		void Draw(Transform* _pTransfrom);
		void DrawSkinAnime(Transform* _pTransform, FbxTime _time);

	private:
		/// <summary>
		/// ���_�o�b�t�@�̏�����
		/// </summary>
		void InitializeVertexBuffer(ID3D11Device* _pDevice) override;
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@�̏�����
		/// </summary>
		void InitializeIndexBuffer(ID3D11Device* _pDevice) override;
		/// <summary>
		/// �R���X�^���g�o�b�t�@�̏�����
		/// </summary>
		void InitializeConstantBuffer(ID3D11Device* _pDevice) override;

		/// <summary>
		/// �}�e���A���̏�����
		/// </summary>
		void InitializeMaterial();
		/// <summary>
		/// �e�N�X�`���̏�����
		/// </summary>
		void InitializeTexture(FbxSurfaceMaterial* _pMaterial, const DWORD _i);
		/// <summary>
		/// �����̏�����
		/// </summary>
		void InitializeSkelton();


	private:
		uint32_t vertexCount_;  // ���_��
		uint32_t polygonCount_;  // �|���S����
		uint32_t indexCount_;  // �C���f�b�N�X��
		uint32_t materialCount_;  // �}�e���A����
		uint32_t polygonVertexCount_;  // �|���S���̒��_��

		FbxNode* pNode_;  // Fbx�m�[�h���
		Material* pMaterial_;  // �}�e���A��
		FbxMesh* pMesh_;  // ���b�V��
		FbxSkin* pSkin_;  // �X�L�����b�V����� (�X�L�����b�V���A�j���[�V�����̃f�[�^)
		FbxCluster** ppCluster_;  // �N���X�^��� (�֐ߎ��Ɋ֘A����ꂽ���_���)
		int boneCount_;  // FBX �Ɋ܂܂�Ă���֐߂̐�
		Bone* pBones_;  // �e�֐߂̏��z��
		std::unordered_map<std::string, Bone*> boneNamePair_;  // �֐ߖ��Ƃ̃y�A
		Weight* pWeights_;  // �E�F�C�g��� (���_�ɑ΂���֐߂̉e���x����)
		Vertex* pVertexes_;  // ���_���
	};

}

