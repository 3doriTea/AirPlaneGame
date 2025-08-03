#include "CameraResource.h"
#include "WindowContextResource.h"
#include "GameObject.h"
#include "ReleaseUtility.h"
#include "SceneSystem.h"
mtgb::CameraResource::CameraResource()
    :pCamera_{nullptr}
{
}
mtgb::CameraResource::~CameraResource()
{
    SAFE_DELETE(pCamera_);
}
mtgb::CameraResource::CameraResource(const CameraResource& other)
    :pCamera_(other.pCamera_)
{
    
}

void mtgb::CameraResource::Initialize(WindowContext _windowContext)
{
}

void mtgb::CameraResource::SetResource()
{
    Game::System<SceneSystem>().GetActiveScene()->SetCameraGameObject(pCamera_);
}

mtgb::WindowContextResource* mtgb::CameraResource::Clone() const
{
    return new CameraResource(*this);
}
