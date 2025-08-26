#include "TrailEmitterSystem.h"

using namespace mtgb;

TrailEmitterSystem::TrailEmitterSystem()
{
}

TrailEmitterSystem::~TrailEmitterSystem()
{
}

void TrailEmitterSystem::Initialize()
{
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
	Vector3 camDir = Game::System<CameraSystem>().GetTransform().Forward();

	for (auto trail : trails_)
	{
		//Vector3 viewDir{ }
		std::vector<yz::Vtx> ribbon;
		trail.BuildRibbon({ camDir.x, camDir.y, camDir.z }, ribbon);
		if (ribbon.size() < 4) continue;

		//D3D11_MAPPED_SUBRESOURCE
	}
}

void TrailEmitterSystem::Release()
{
}
