#include "Terrain.h"

using namespace mtgb;

Terrain::Terrain() : GameObject(GameObjectBuilder()
	.SetName("Terrain")
	.Build())
{
}

Terrain::~Terrain()
{
}

void Terrain::Update()
{
}

void Terrain::Draw() const
{
	Draw::GroundPlane();
}
