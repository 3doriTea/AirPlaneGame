#pragma once
#include "WindowContextResource.h"
#include "Handlers.h"
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

		void SetHCamera(CameraHandleInScene _hCamera);
		//void SetCamera(GameObject* obj);
	private:
		//GameObject* pCamera_;
		CameraHandleInScene hCamera_;
	};
}