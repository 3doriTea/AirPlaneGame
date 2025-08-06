#include "SkySphere.h"
#include "Bullet.h"
#include "../mtgb/DirectX11Draw.h"
#include "DoubleWindow.h"
#include "Inspector.h"

using namespace mtgb;

namespace
{
	static const float SkySphere_SPEED{ 0.1f };
	int hText;
	int timer = 0;
	TimerHandle timerHandle;
}

SkySphere::SkySphere() : GameObject(GameObjectBuilder()
							  .SetName("SkySphere")
							  .SetPosition(Vector3(0, 0, 0))
							  .SetRotate(Quaternion::Identity())
							  .SetScale({ 1, 1, 1 })
							  .Build()),
	pTransform_{ Component<Transform>() }
{
	hModel_ = Fbx::Load("Model/sky_sphere.fbx");
}

SkySphere::SkySphere(mtgb::WindowContext context)
	:SkySphere()
{
	context_ = context;
}

SkySphere::~SkySphere()
{
}

void SkySphere::Update()
{
}

void SkySphere::Draw() const
{
	Draw::FBXModel(hModel_, *pTransform_, 0);
}
