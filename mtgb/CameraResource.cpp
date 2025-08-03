#include "CameraResource.h"
#include "WindowContextResource.h"
#include "GameObject.h"
#include "ReleaseUtility.h"
#include "SceneSystem.h"
#include "MTAssert.h"
mtgb::CameraResource::CameraResource()
    :pCamera_{nullptr}
{
}
mtgb::CameraResource::~CameraResource()
{
    SAFE_DELETE(pCamera_);
}
mtgb::CameraResource::CameraResource(const CameraResource& other)
{
    if (other.pCamera_ != nullptr)
    {
        pCamera_ = new GameObject(*other.pCamera_);
    }
    else
    {
        pCamera_ = nullptr;
    }
}

void mtgb::CameraResource::Initialize(WindowContext _windowContext)
{
}

void mtgb::CameraResource::SetResource()
{
    /*massert(pCamera_
        && "pCamera_‚Ínullptr‚Å‚· @CameraResource::SetResource");*/
    if (pCamera_ != nullptr)
    {
        Game::System<SceneSystem>().GetActiveScene()->SetCameraGameObject(pCamera_);
    }
}

mtgb::WindowContextResource* mtgb::CameraResource::Clone() const
{
    return new CameraResource(*this);
}

void mtgb::CameraResource::SetCamera(GameObject* obj)
{
    pCamera_ = obj;
}
