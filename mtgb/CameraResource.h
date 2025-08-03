#pragma once
#include "WindowContextResource.h"

namespace mtgb
{
	class GameObject;
	/// <summary>
	/// ウィンドウごとのカメラ
	/// </summary>
	class CameraResource : public WindowContextResource
	{
	public:
		CameraResource();
		~CameraResource();
		CameraResource(const CameraResource& other);
		// WindowContextResource を介して継承されました
		void Initialize(WindowContext _windowContext) override;
		/// <summary>
		/// カメラを切り替える
		/// </summary>
		void SetResource() override;
		WindowContextResource* Clone() const override;

		void SetCamera(GameObject* obj);
	private:
		GameObject* pCamera_;
	};
}