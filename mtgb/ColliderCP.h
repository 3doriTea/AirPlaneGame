#pragma once
#include "ComponentPool.h"
#include "Collider.h"

#include <vector>
#include "WindowContext.h"
#include "RectContainsInfo.h"
#include "RectF.h"
namespace mtgb
{
	class Collider;
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
		void RectContains(const RectF& _rect, const std::string& _name, std::vector<RectContainsInfo>* _info, WindowContext _context);
	};
}
