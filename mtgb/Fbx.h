#pragma once
#include <vector>
#include <fbxsdk.h>
#include "IModelPack.h"

#pragma comment(lib, "LibFbxSDK-MT.lib")
#pragma comment(lib, "LibXml2-MT.lib")
#pragma comment(lib, "zlib-MT.lib")


namespace mtgb
{
	/// <summary>
	/// 3D���f����Fbx��ǂݍ��݂���N���X
	/// </summary>
	class Fbx : public IModelPack
	{
		friend class FbxParts;

	public:
		Fbx();
		~Fbx();

		/// <summary>
		/// FbxManager���擾����
		/// </summary>
		/// <returns>FbxManager�̃|�C���^</returns>
		inline FbxManager* GetFbxManager() { return pFbxManager_; }
		/// <summary>
		/// FbxScene���擾����
		/// </summary>
		/// <returns>FbxScene�̃|�C���^</returns>
		inline FbxScene* GetFbxScene() { return pFbxScene_; }

		/// <summary>
		/// Fbx�t�@�C����ǂݍ��݂���
		/// </summary>
		/// <param name="_fileName">3D���f���̃t�@�C����</param>
		void Load(const std::string& _fileName) override;
		/// <summary>
		/// Fbx���f����`�悷��
		/// </summary>
		/// <param name="_pTransform">���W�n</param>
		/// <param name="_frame">�A�j���[�V�����t���[��</param>
		void Draw(Transform* _pTransform, int _frame) override;
		/// <summary>
		/// �������
		/// </summary>
		void Release() override;

	private:
		std::vector<FbxParts*> pParts_;  // �������邩������Ȃ��p�[�c

		FbxManager* pFbxManager_;  // Fbx�t�@�C���������{��
		FbxScene* pFbxScene_;  // Fbx�t�@�C�����̃V�[��

		FbxTime::EMode frameRate_;  // �A�j���[�V�����t���[�����[�g
		float animationSpeed_;  // �A�j���[�V�����̍Đ����x
		int startFrame_;  // �A�j���[�V�����ŏ��̃t���[��
		int endFrame_;  // �A�j���[�V�����Ō�̃t���[��

		/// <summary>
		/// �m�[�h�̒��g�𒲂ׂ�
		/// </summary>
		/// <param name="_pNode">���ׂ����m�[�h</param>
		/// <param name="_parts">�p�[�c���X�g</param>
		void CheckNode(FbxNode* _pNode, std::vector<FbxParts*>& _parts);
	};
}
