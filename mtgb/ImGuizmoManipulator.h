#pragma once
#include "ImGuiShowable.h"
#include "Matrix4x4.h"
#include "Handlers.h"
#include <optional>
#include "../ImGui/imgui.h"
#include "../ImGui\ImGuizmo.h"


namespace mtgb
{

	class GameObject;
	class Transform;
	struct Vector3;

	class ImGuizmoManipulator : public ImGuiShowable
	{
		friend class ImGuiRenderer;
	public:
		ImGuizmoManipulator();
		~ImGuizmoManipulator();

		void SetCamera();
		void Initialize();
		void ShowImGui() override;
		void UpdateCamera(const char* _name);
		bool IsMouseInWindow(const char* _name);
		void GetMouseRay(Vector3* _near, Vector3* _far);
		void SelectTransform();
		void FollowTarget();
		std::optional<ImVec2> WorldToImGui(const Vector3& _vec);
	private:
		
		void DrawTransformGuizmo();
		void Calculate();

		void SpinCamera(float _distance);
		
		void InitializeSpinAnglesFromCurrentPosition();
		ImGuizmo::OPERATION operation_;
		ImGuizmo::MODE mode_;
		GameObject* pCamera_;
		Transform* pCameraTransform_;
		Transform* pTargetTransform_;
		float angleX_, angleY_;
		float spinAngleX_,spinAngleY_;
		CameraHandleInScene hCamera_;
		uintptr_t currId_;
	
		enum class CameraOperation
		{
			None,
			Translate,
			Rotate,
			Spin
		};
		CameraOperation cameraOperation_;
		bool updatingCameraTransform_;
		bool followTarget_;
		bool adjustTargetDirection_; // カメラがターゲットに追従する際、カメラの向きをターゲットの向きに合わせるか
		float spinDistance_;
		float followDistance_;
		float spinSpeed_;
		float moveSpeed_;
		float rotateSensitivity_;
		float thickness_;
		float worldMat_[16], viewMat_[16], projMat_[16];
		Matrix4x4 worldMatrix4x4, viewMatrix4x4_, projMatrix4x4_;
		DirectX::XMFLOAT4X4 float4x4_;

	};
}
