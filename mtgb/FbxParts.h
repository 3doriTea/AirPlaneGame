#pragma once
#include "cmtgb.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "IShader.h"
#include "Texture2D.h"
#include <unordered_map>
#include <fbxsdk.h>


namespace fbxsdk
{/*
	class FbxMesh;
	class FbxSkin;*/
	class FbxCluster;
}

namespace mtgb
{
	class Fbx;
	class Transform;
	class DirectX11Draw;

	class FbxParts : public IShader
	{
		friend DirectX11Draw;
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
			Vector4 g_cameraPosition; // �J�����̈ʒu�i�n�C���C�g�̌v�Z�ɕK�v�j
			FLOAT g_shininess;     // �X�y�L�����̋���
			BOOL g_isTexture;  // �e�N�X�`���̗L��
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
		FbxParts(FbxNode* _parent);
		~FbxParts();

		void Initialize() override;

		/// <summary>
		/// ���f����`�悷��
		/// </summary>
		/// <param name="_transform">���W�n</param>
		void Draw(const Transform& _transform);
		/// <summary>
		/// �{�[������Ń��f����`�悷��
		/// </summary>
		/// <param name="_transform">���W�n</param>
		/// <param name="_time">�A�j���[�V�����t���[��</param>
		void DrawSkinAnimation(const Transform& _transform, FbxTime _time);
		/// <summary>
		/// �{�[������Ń��f����`�悷��
		/// </summary>
		/// <param name="_takeName"></param>
		/// <param name="_transform">���W�n</param>
		/// <param name="_time">�t���[��</param>
		void DrawSkinAnimation(const std::string& _takeName, const Transform& _transform, FbxTime _time);
		/// <summary>
		/// �{�[�������Ń��f����`�悷��
		/// </summary>
		/// <param name="_transform">���W�n</param>
		/// <param name="_time">�t���[��</param>
		void DrawMeshAnimation(const Transform& _transform, FbxTime _time);

		/// <summary>
		/// �����Ƀ{�[���̃A�j���[�V���������̂Ƃ��̍��W���擾����
		/// </summary>
		/// <param name="_boneName">���O</param>
		/// <param name="_pPosition">���W�̎Q�Ɠn��</param>
		/// <returns>�{�[���̎擾�ɐ������� true / false</returns>
		bool TryGetBonePosition(const std::string& _boneName, Vector3* _pPosition);
		/// <summary>
		/// �����Ƀ{�[���̃A�j���[�V�������̍��W���擾����
		/// </summary>
		/// <param name="_boneName">���O</param>
		/// <param name="_pPosition">���W�̎Q�Ɠn��</param>
		/// <returns>�{�[���̎擾�ɐ������� true / false</returns>
		bool TryGetBonePositionAtNow(const std::string& _boneName, Vector3* _pPosition);

		/// <summary>
		/// Fbx�̃X�L�����擾
		/// </summary>
		/// <returns>�X�L���̃|�C���^</returns>
		FbxSkin* GetSkin() { return pSkin_; }

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
		DWORD** ppIndexData_;  // �C���f�b�N�X���

		ID3D11Buffer** ppIndexBuffer_;
	};

}

