#include "CameraSystem.h"
#include "SceneSystem.h"
#include <algorithm>
#include "MTAssert.h"
#include "Screen.h"
#include <DirectXMath.h>

#include "Debug.h"


namespace
{
	static const float DEFAULT_FOV{ 60.0f };  // �f�t�H���g�̎���p (Field Of View)
	static const float DEFAULT_NEAR{ 0.1f };  // �f�t�H���g�̃j���[����
	static const float DEFAULT_FAR{ 100.0f };  // �f�t�H���g�̃t�@�[����
}

mtgb::CameraSystem::CameraSystem() :
	pTransforms_{},
	fov_{ DEFAULT_FOV },
	near_{ DEFAULT_NEAR },
	far_{ DEFAULT_FAR },
	hCurrentCamera_{ INVALID_HANDLE }
{
}

mtgb::CameraSystem::~CameraSystem()
{
}

void mtgb::CameraSystem::Initialize()
{
}

void mtgb::CameraSystem::Update()
{
}

mtgb::CameraHandleInScene mtgb::CameraSystem::RegisterDrawCamera(Transform* pCameraTransform_)
{
	CameraHandleInScene handle{ INVALID_HANDLE };

	// �����J���������o�^�Ȃ�
	if (pTransforms_.size() == 0)
	{
		handle = 0;
		SetDrawCamera(handle);
		pTransforms_.push_back(pCameraTransform_);
		return handle;
	}

	auto itr{ std::find(pTransforms_.begin(), pTransforms_.end(), pCameraTransform_) };

	// ������Ȃ�����
	if (itr == pTransforms_.end())
	{
		handle = pTransforms_.size();
		pTransforms_.push_back(pCameraTransform_);
	}
	else  // ���ɂ���
	{
		// �C���f�b�N�X��Ԃ�
		handle = itr - pTransforms_.begin();
	}

	return handle;
}

void mtgb::CameraSystem::ClearDrawCameraAll()
{
	pTransforms_.clear();
}

void mtgb::CameraSystem::UnregisterDrawCamera(const Transform* pCameraTransform_)
{
	for (auto itr = pTransforms_.begin(); itr != pTransforms_.end(); itr++)
	{
		if ((*itr) == pCameraTransform_)
		{
			(*itr) = nullptr;
			return;  // ��v������nullptr�ɂ��ĉ�A
		}
	}
}

const mtgb::Transform& mtgb::CameraSystem::GetTransform() const
{
	massert(0 <= hCurrentCamera_ && hCurrentCamera_ < pTransforms_.size()
		&& "�J�����n���h���������ł��B");

	Transform* pTransform{ pTransforms_[hCurrentCamera_] };

	LOGF("pTrans = %p, (%f, %f, %f)\n", pTransform, pTransform->position_.x, pTransform->position_.y, pTransform->position_.z);

	massert(pTransform != nullptr
		&& "���ɖ��������ꂽ�J�������Q�Ƃ���܂����B");

	return *pTransform;
}

void mtgb::CameraSystem::GetViewMatrix(Matrix4x4* _pView) const
{
	const Transform& cameraTransform{ GetTransform() };

	Vector4 vEyePt{ cameraTransform.position_ };  // �J�����i���_�j�ʒu
	Vector4 vLookatPt{ cameraTransform.Forward() + cameraTransform.position_ };  // �����ʒu
	Vector4 vUpVec{ Vector3::Up() };  // ����ʒu

	*_pView = XMMatrixLookAtLH(vEyePt, vLookatPt, vUpVec);
}

void mtgb::CameraSystem::GetProjMatrix(Matrix4x4* _pProj) const
{
	using DirectX::XMMatrixPerspectiveFovLH;
	using DirectX::XMConvertToRadians;

	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	*_pProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(fov_), 
		static_cast<float>(SCREEN_SIZE.x) / static_cast<float>(SCREEN_SIZE.y),
		near_,
		far_);
}

void mtgb::CameraSystem::GetPosition(Vector4* _pPosition) const
{
	*_pPosition = GetTransform().position_;
}
