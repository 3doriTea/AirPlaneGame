#include "CameraSystem.h"

mtgb::CameraSystem::CameraSystem()
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

CameraHandleInScene mtgb::CameraSystem::AddDrawCamera()
{
    return CameraHandleInScene();
}

void mtgb::CameraSystem::SetFOV(const float _angle)
{
}

void mtgb::CameraSystem::SetDrawCamera(const int _cameraNumber)
{
}

const Transform& mtgb::CameraSystem::GetTransform() const
{
    // TODO: return �X�e�[�g�����g�������ɑ}�����܂�
}

void mtgb::CameraSystem::GetViewMatrix(Matrix4x4* _pView) const
{
}

void mtgb::CameraSystem::GetProjMatrix(Matrix4x4* _pProj) const
{
}
