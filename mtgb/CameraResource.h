#pragma once
#include "WindowContextResource.h"

namespace mtgb
{
	class GameObject;
	/// <summary>
	/// �E�B���h�E���Ƃ̃J����
	/// </summary>
	class CameraResource : public WindowContextResource
	{
	public:
		CameraResource();
		~CameraResource();
		CameraResource(const CameraResource& other);
		// WindowContextResource ����Čp������܂���
		void Initialize(WindowContext _windowContext) override;
		/// <summary>
		/// �J������؂�ւ���
		/// </summary>
		void SetResource() override;
		WindowContextResource* Clone() const override;

		void SetCamera(GameObject* obj);
	private:
		GameObject* pCamera_;
	};
}