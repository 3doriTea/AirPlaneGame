#include "ImGuizmoManipulator.h"
#include "GameObject.h"
#include "Vector3.h"
#include "Transform.h"
#include "Game.h"
#include "ISystem.h"
#include "../ImGui/imgui.h"
#include "../ImGui\imgui_internal.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "CameraSystem.h"
#include "InputData.h"
#include "WindowContextUtil.h"
#include "GameTime.h"
#include "QuatToEuler.h"
void mtgb::ImGuizmoManipulator::DrawTransformGuizmo()
{
	if (!pTargetTransform_)
	{
		return;
	}
	uintptr_t ptrId = reinterpret_cast<uintptr_t>(pTargetTransform_);

	Calculate();

	ImGui::PushID(&ptrId);



	ImVec2 pos = ImGui::GetWindowPos();
	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();

	//ギズモ表示
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
	ImGuizmo::SetRect(pos.x, pos.y + tabBarHeight, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);




	if (ImGuizmo::Manipulate(viewMat_, projMat_, operation_, mode_, worldMat_))
	{
		//編集されたworldMatからposition,rotation,scaleに分解
		DirectX::XMMATRIX mat = DirectX::XMMATRIX(
			worldMat_[0], worldMat_[1], worldMat_[2], worldMat_[3],
			worldMat_[4], worldMat_[5], worldMat_[6], worldMat_[7],
			worldMat_[8], worldMat_[9], worldMat_[10], worldMat_[11],
			worldMat_[12], worldMat_[13], worldMat_[14], worldMat_[15]
		);

		DirectX::XMVECTOR scale, trans;
		bool result = DirectX::XMMatrixDecompose(&scale, &pTargetTransform_->rotate.v, &trans, mat);
		massert(result
			&& "XMMatrixDecomposeに失敗 @MTImGui::DrawTransformGuizmo");

		DirectX::XMStoreFloat3(&pTargetTransform_->position, trans);
		DirectX::XMStoreFloat3(&pTargetTransform_->scale, scale);

	}
	ImGui::PopID();
}

void mtgb::ImGuizmoManipulator::Calculate()
{
	//float[16]の配列を作成
	pTargetTransform_->GenerateWorldMatrix(&worldMatrix4x4);
	Game::System<mtgb::CameraSystem>().GetViewMatrix(&viewMatrix4x4_);
	Game::System<mtgb::CameraSystem>().GetProjMatrix(&projMatrix4x4_);


	//ワールド行列
	DirectX::XMStoreFloat4x4(&float4x4_, worldMatrix4x4);
	memcpy(worldMat_, &float4x4_, sizeof(worldMat_));

	//ビュー行列
	DirectX::XMStoreFloat4x4(&float4x4_, viewMatrix4x4_);
	memcpy(viewMat_, &float4x4_, sizeof(viewMat_));

	//プロジェクション行列
	DirectX::XMStoreFloat4x4(&float4x4_, projMatrix4x4_);
	memcpy(projMat_, &float4x4_, sizeof(projMat_));
}

void mtgb::ImGuizmoManipulator::SpinCamera()
{
	pTargetTransform_->position;
	Vector3 cameraForward = pCameraTransform_->Forward();
	Vector3 center = pCameraTransform_->position + (cameraForward * distance_);

	float theta = spinAngleX_;
	float phi = spinAngleY_;

	Vector3 offset;

	offset.x = distance_ * sinf(theta) * cos(phi);
	offset.y = -distance_ * cos(theta);
	offset.z = -distance_ * sin(theta) * sin(phi);

	pCameraTransform_->position =  center + offset;

	Vector3 lookDir = center - pCameraTransform_->position;
	pCameraTransform_->rotate = Quaternion::LookRotation(lookDir, pCameraTransform_->Up());
}

void mtgb::ImGuizmoManipulator::InitializeSpinAnglesFromCurrentPosition()
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB
	// 球面座標から直行直線座標への変換
	
	// 現在のカメラ位置から回転中心を計算
	Vector3 cameraForward = pCameraTransform_->Forward();
	Vector3 center = pCameraTransform_->position + (cameraForward * distance_);
	
	// カメラ位置から中心へのベクトル
	Vector3 offset = pCameraTransform_->position - center;
	
	// 球面座標系の角度を逆算
	float r = offset.Size();
	if (r > 0.001f) // ゼロ除算を避ける
	{
		// θ (polar angle): Y軸からの角度
		spinAngleX_ = acosf(-offset.y / r);
		
		// φ (azimuthal angle): X-Z平面での角度
		spinAngleY_ = atan2f(-offset.z, offset.x);
		
		// 距離も更新
		distance_ = r;
	}
	else
	{
		// カメラが中心にいる場合のデフォルト値
		spinAngleX_ = DirectX::XMConvertToRadians(90.0f); // 水平
		spinAngleY_ = 0.0f;
	}
}

mtgb::ImGuizmoManipulator::ImGuizmoManipulator()
	:ImGuiShowable("Manipulater", ShowType::SceneView)
	, operation_{ ImGuizmo::TRANSLATE }
	, mode_{ ImGuizmo::LOCAL }
	, angleX_{ 0.0f }
	, angleY_{ 0.0f }
	, distance_{5.0f}
	, moveSpeed_{3.0f}
	, spinSpeed_{1.0f}
	, rotateSensitivity_{0.01f}
	, updatingCameraTransform_{ false }
	, cameraOperation_{ CameraOperation::None }
{
	pCamera_ = new GameObject(
		GameObjectBuilder()
		.SetPosition({ 0,0,0 })
		.SetName("Camera")
		.Build());

	pCameraTransform_ = &Game::System<TransformCP>().Get(pCamera_->GetEntityId());
	hCamera_ = Game::System<CameraSystem>().RegisterDrawCamera(pCameraTransform_);
}

void mtgb::ImGuizmoManipulator::SetCamera()
{
	Game::System<CameraSystem>().SetDrawCamera(hCamera_);
}

void mtgb::ImGuizmoManipulator::Initialize()
{
}

void mtgb::ImGuizmoManipulator::ShowImGui()
{
	//ImGuizmoの操作モードを指定


	if (InputUtil::GetMouseDown(MouseCode::Left))
	{
		if ((!ImGuizmo::IsViewManipulateHovered() && !ImGuizmo::IsOver()) || !pTargetTransform_)
		{
			SelectTransform();
		}
	}

	DrawTransformGuizmo();


	if (ImGui::RadioButton("Translate", operation_ == ImGuizmo::TRANSLATE))
	{
		operation_ = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", operation_ == ImGuizmo::ROTATE))
	{
		operation_ = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", operation_ == ImGuizmo::SCALE))
	{
		operation_ = ImGuizmo::SCALE;
	}

	if (ImGui::RadioButton("Local", mode_ == ImGuizmo::LOCAL))
	{
		mode_ = ImGuizmo::LOCAL;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("World", mode_ == ImGuizmo::WORLD))
	{
		mode_ = ImGuizmo::WORLD;
	}

	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 localPos = ImVec2(mousePos.x - windowPos.x, mousePos.y - windowPos.y);
	int mousePosInt[2] = { localPos.x ,localPos.y };
	ImGui::InputInt2("mouse", mousePosInt);

	MTImGui::Instance().TypedShow(&pCameraTransform_->position, "cameraPos");
	MTImGui::Instance().TypedShow(&pCameraTransform_->rotate, "cameraRot");
}

void mtgb::ImGuizmoManipulator::UpdateCamera(const char* _name)
{
	constexpr float ANGLE_SPEED{ DirectX::XMConvertToRadians(100.f) };
	if (!ImGui::IsWindowFocused())
	{
		updatingCameraTransform_ = false;
		return;
	}
	if (IsMouseInWindow(_name) && !updatingCameraTransform_)
	{
		if (InputUtil::GetMouseDown(MouseCode::Right) || InputUtil::GetMouseDown(MouseCode::Middle) || InputUtil::GetMouseDown(MouseCode::Left))
		{
			updatingCameraTransform_ = true;

			if (InputUtil::GetMouseDown(MouseCode::Middle))
			{
				cameraOperation_ = CameraOperation::Translate;
			}
			else if (InputUtil::GetMouseDown(MouseCode::Right))
			{
				cameraOperation_ = CameraOperation::Rotate;
				// Rotate操作用の角度初期化
				Vector3 currentEuler = QuatToEuler(pCameraTransform_->rotate);
			}
			else if (InputUtil::GetMouseDown(MouseCode::Left) && InputUtil::GetKey(KeyCode::LeftMenu))
			{
				cameraOperation_ = CameraOperation::Spin;
				// Spin操作用の角度初期化 - 現在のカメラ位置から逆算
				InitializeSpinAnglesFromCurrentPosition();
			}
			else
			{
				cameraOperation_ = CameraOperation::None;
			}
		}
	}
	if (!updatingCameraTransform_)
	{
		return;
	}


	if (InputUtil::GetMouseUp(MouseCode::Right) || InputUtil::GetMouseUp(MouseCode::Middle) || InputUtil::GetMouseUp(MouseCode::Left))
	{
		updatingCameraTransform_ = false;
		return;
	}


	if (cameraOperation_ == CameraOperation::Rotate)
	{
		Vector3 mouseMove = InputUtil::GetMouseMove();

		pCameraTransform_->rotate *= Quaternion::Euler({ mouseMove.y * rotateSensitivity_, mouseMove.x * rotateSensitivity_, 0.0f });
	}

	if (cameraOperation_ == CameraOperation::Translate)
	{
		Vector3 mouseMove = InputUtil::GetMouseMove();
		pCameraTransform_->position.x -= mouseMove.x * moveSpeed_ * Time::DeltaTimeF();
		pCameraTransform_->position.y += mouseMove.y * moveSpeed_ * Time::DeltaTimeF();
	}


	if (cameraOperation_ == CameraOperation::Spin)
	{
		Vector3 mouseMove = InputUtil::GetMouseMove();
		spinAngleX_ += mouseMove.y * spinSpeed_ * Time::DeltaTimeF();
		spinAngleY_ += mouseMove.x * spinSpeed_ * Time::DeltaTimeF();

		SpinCamera();
	}

}