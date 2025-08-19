#pragma once
#include "ComponentPool.h"
#include "Collider.h"

namespace mtgb
{
	class Collider;
	struct Vector3;
	class ColliderCP : public ComponentPool<Collider, false>
	{
	public:
		ColliderCP();
		~ColliderCP();

		void Start() override;
		void Update() override;

		void TestDraw() const;
		EntityId RaycastHit(const Vector3& _origin, const Vector3& _dir, float dist);

	};
}
