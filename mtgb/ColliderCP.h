#pragma once
#include "ComponentPool.h"
#include "Collider.h"

#include <vector>
#include "WindowContext.h"
namespace mtgb
{
	class Collider;
	struct Vector3;
	struct RectInt;
	class GameObject;
	class ColliderCP : public ComponentPool<Collider, false>
	{
	public:
		ColliderCP();
		~ColliderCP();

		void Start() override;
		void Update() override;

		void TestDraw() const;
		EntityId RaycastHit(const Vector3& _origin, const Vector3& _dir, float dist);
		void RectContains(const RectInt& _rect, const std::string& _name, std::vector<GameObject*>* _pRectContainsGameObject, WindowContext _context);
	};
}
