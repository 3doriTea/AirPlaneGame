#pragma once
#include "ISystem.h"
#include "Transform.h"
#include "Matrix4x4.h"
#include "Handlers.h"
#include "Vector4.h"


namespace mtgb
{
	class CameraSystem : public ISystem
	{
	public:
		CameraSystem();
		~CameraSystem();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// �J�����ƂȂ���W�n��o�^����
		/// </summary>
		/// <param name="pCameraTransform_">���W�n�̃|�C���^</param>
		/// <returns>�J�����n���h��</returns>
		CameraHandleInScene RegisterDrawCamera(Transform* pCameraTransform_);
		/// <summary>
		/// �o�^��������
		/// </summary>
		/// <param name="pCameraTransform_">����������W�n�̃|�C���^</param>
		void UnregisterDrawCamera(const Transform* pCameraTransform_);
		/// <summary>
		/// ���ׂĂ̕`��J�������N���A����
		/// </summary>
		void ClearDrawCameraAll();

		/// <summary>
		/// FOV���Z�b�g����
		/// </summary>
		/// <param name="_angle">����p�x(Degree)</param>
		void SetFOV(const float _angle) { fov_ = _angle; }
		/// <summary>
		/// �`��ΏۂƂȂ�J�������w�肷��
		/// </summary>
		/// <param name="_hCamera">�w�肷��J�����̃n���h��</param>
		void SetDrawCamera(const CameraHandleInScene _hCamera) { hCurrentCamera_ = _hCamera; }
		/// <summary>
		/// ���ݕ`��ΏۂƂȂ��Ă���J�����̃n���h�����擾
		/// </summary>
		/// <returns>�J�����̃n���h��</returns>
		const CameraHandleInScene GetDrawCamera() const { return hCurrentCamera_; }


		const Transform& GetTransform() const;
		void GetViewMatrix(Matrix4x4* _pView) const;
		void GetProjMatrix(Matrix4x4* _pProj) const;
		void GetPosition(Vector4* _pPosition) const;

	private:
		std::vector<Transform*> pTransforms_;
		float fov_;
		float near_;
		float far_;
		CameraHandleInScene hCurrentCamera_;
	};
}
