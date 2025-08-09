#pragma once
#include "ComponentPool.h"
#include "Collider.h"

namespace mtgb
{
	class Collider;

	class ColliderCP : public ComponentPool<Collider, false>
	{
	public:
		ColliderCP();
		~ColliderCP();

		void Start() override;
		void Update() override;
	};
}
