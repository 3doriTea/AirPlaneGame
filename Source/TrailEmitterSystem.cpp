#include "TrailEmitterSystem.h"
#include <DirectXMath.h>

using namespace mtgb;

namespace
{
	const int POINTS_MAX{ 3000 };
	const float WIDTH{ 0.3f };
}

TrailEmitterSystem::TrailEmitterSystem() :
	trail_{},
	pointsMax_{ POINTS_MAX },
	width_{ WIDTH }
{
}

TrailEmitterSystem::~TrailEmitterSystem()
{
}

void TrailEmitterSystem::Initialize()
{
	trail_.Initialize();


	using DirectX::XMFLOAT4;
	using namespace yz;

	TrailEmitter a(f3(0, 0, 100), XMFLOAT4(0.30f, 0.80f, 1.00f, 0.90f), 0.32f);
	a.SetPhase(10.f); a.SetRadius(2.2f); a.SetMotionSpeed(1.00f); a.SetBaseHeight(0.4f); a.SetEmitInterval(0.0001f);
	/*TrailEmitter b(f3(0, 0, 3), XMFLOAT4(1.00f, 0.55f, 0.25f, 0.85f), 0.26f);
	b.SetPhase(1.8f); b.SetRadius(2.6f); b.SetMotionSpeed(0.92f); b.SetBaseHeight(0.6f); b.SetEmitInterval(0.020f);
	TrailEmitter c(f3(0, 0, 3), XMFLOAT4(0.70f, 0.60f, 1.00f, 0.80f), 0.22f);
	c.SetPhase(3.3f); c.SetRadius(2.0f); c.SetMotionSpeed(1.12f); c.SetBaseHeight(0.8f); c.SetEmitInterval(0.016f);*/

	trails_ = { a, };
}

void TrailEmitterSystem::Update()
{
	float deltaTime{ Time::DeltaTimeF() };
	for (auto& trail : trails_)
	{
		trail.SetWidth(width_);
		trail.SetMaxPoints(pointsMax_);
		trail.Update(deltaTime);
	}
}

void TrailEmitterSystem::Render()
{
	//Vector3 camDir = Game::System<CameraSystem>().GetTransform().Forward();

	for (const auto& trail : trails_)
	{
		trail_.Draw(trail);
		////Vector3 viewDir{ }
		//std::vector<yz::Vtx> ribbon;
		//trail.BuildRibbon({ camDir.x, camDir.y, camDir.z }, ribbon);
		//if (ribbon.size() < 4) continue;

		////D3D11_MAPPED_SUBRESOURCE
	}
}

void TrailEmitterSystem::Release()
{
}
