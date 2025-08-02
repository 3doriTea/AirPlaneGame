#pragma once
#include "ISystem.h"
#include "Transform.h"
#include "Matrix4x4.h"
#include "Handlers.h"
#include "Transform.h"

namespace mtgb
{
	class CameraSystem : public ISystem
	{
	public:
		CameraSystem();
		~CameraSystem();

		void Initialize() override;
		void Update() override;

		CameraHandleInScene AddDrawCamera();

		void SetFOV(const float _angle);
		void SetDrawCamera(const int _cameraNumber);

		const Transform& GetTransform() const;
		void GetViewMatrix(Matrix4x4* _pView) const;
		void GetProjMatrix(Matrix4x4* _pProj) const;

	private:
		std::vector<Transform*> pTransforms_;
	};
}
