#include "CameraResource.h"
#include "WindowContextResource.h"
#include "GameObject.h"
#include "ReleaseUtility.h"
#include "SceneSystem.h"
#include "MTAssert.h"
#include "CameraSystem.h"
mtgb::CameraResource::CameraResource()
    //:pCamera_{nullptr}
{
}
mtgb::CameraResource::~CameraResource()
{
    //SAFE_DELETE(pCamera_);
}
mtgb::CameraResource::CameraResource(const CameraResource& other)
    :WindowContextResource(other)
    , hCamera_(other.hCamera_)
{
    /*if (other.pCamera_ != nullptr)
    {
        pCamera_ = new GameObject(*other.pCamera_);
    }
    else
    {
        pCamera_ = nullptr;
    }*/
}

void mtgb::CameraResource::Initialize(WindowContext _windowContext)
{
}

void mtgb::CameraResource::SetResource()
{
    /*massert(pCamera_
        && "pCamera_‚Ínullptr‚Å‚· @CameraResource::SetResource");*/
    /*if (pCamera_ != nullptr)
    {
        Game::System<SceneSystem>().GetActiveScene()->RegisterCameraGameObject(pCamera_);
    }*/
    Game::System<CameraSystem>().SetDrawCamera(hCamera_);
}

mtgb::WindowContextResource* mtgb::CameraResource::Clone() const
{
    return new CameraResource(*this);
}

void mtgb::CameraResource::SetHCamera(CameraHandleInScene _hCamera)
{
    hCamera_ = _hCamera;
}

//void mtgb::CameraResource::SetCamera(GameObject* obj)
//{
//    pCamera_ = obj;
//}
