#include "ImGuizmoManipulator.h"
#include "GameObject.h"
#include "Vector3.h"
#include "Transform.h"
#include "Game.h"
#include "ISystem.h"
//#include "../ImGui/imgui.h"
//#include "../ImGui\ImGuizmo.h"

#include "../ImGui\imgui_internal.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "CameraSystem.h"
#include "InputData.h"
#include "WindowContextUtil.h"
#include "GameTime.h"
#include "QuatToEuler.h"
#include "ImGuiRenderer.h"
#include "MTImGui.h"
#include <cmath>
#include <algorithm>
#include "SceneSystem.h"
namespace
{
	
}

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

std::optional<ImVec2> mtgb::ImGuizmoManipulator::WorldToImGui(const Vector3& _vec)
{
	Game::System<CameraSystem>().GetViewMatrix(&viewMatrix4x4_);
	Game::System<CameraSystem>().GetProjMatrix(&projMatrix4x4_);

	const D3D11_VIEWPORT& viewport = Game::System<ImGuiRenderer>().GetViewport();
	ImVec2 windowPos = ImGui::GetWindowPos();
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;


	Vector3 screenVec = DirectX::XMVector3Project(
		_vec,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		projMatrix4x4_,
		viewMatrix4x4_,
		DirectX::XMMatrixIdentity());

	if (screenVec.z < 0.0f || screenVec.z > 1.0f)
		return std::nullopt;
	//return ImVec2(screenVec.x, screenVec.y );
	return ImVec2(screenVec.x + windowPos.x, screenVec.y + windowPos.y + tabBarHeight);
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

void mtgb::ImGuizmoManipulator::SpinCamera(float _distance)
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB
	// 球面座標から直行直線座標への変換
	
	// 現在のカメラ位置から回転中心を計算
	Vector3 center = Vector3::Zero();
	// ターゲットを中心に回転
	if (pTargetTransform_)
	{
		Vector3 toTarget = pTargetTransform_->position - pCameraTransform_->position;
		center = pTargetTransform_->position + (Vector3::Normalize(-toTarget) * _distance);
	}
	// ターゲットがいない場合は正面を向いて回転
	else
	{
		center = pCameraTransform_->position + (pCameraTransform_->Forward() * _distance);
	}

	// θ (polar angle) : 鉛直方向
	float theta = spinAngleX_;

	// φ (azimuthal angle): 水平方向
	float phi = spinAngleY_;

	// 回転中心からのオフセット
	Vector3 offset;
	
	// 回転中心の方向を向く
	Vector3 lookDir = Vector3::Zero();
	if (_distance <= std::numeric_limits<float>::epsilon())
	{
		// 変換
		offset.x = sinf(theta) * cos(phi);
		offset.y = cos(theta);
		offset.z = sin(theta) * sin(phi);
	}
	else
	{
		// 変換
		offset.x = _distance * sinf(theta) * cos(phi);
		offset.y = -_distance * cos(theta);
		offset.z = -_distance * sin(theta) * sin(phi);

		// 位置を反映
		pCameraTransform_->position =  center + offset;

		lookDir = center - pCameraTransform_->position;
	}

	if (lookDir.Size() == 0.0f)
	{
		// その場回転の時はoffsetの方向を向く
		pCameraTransform_->rotate = Quaternion::LookRotation(offset, pCameraTransform_->Up());
	}
	else
	{
		pCameraTransform_->rotate = Quaternion::LookRotation(lookDir.Normalize(), pCameraTransform_->Up());
	}
}

void mtgb::ImGuizmoManipulator::InitializeSpinAnglesFromCurrentPosition()
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB
	// 直行直線座標から球面座標への変換
	
	// 現在のカメラ位置から回転中心を計算
	Vector3 center = Vector3::Zero();
	//// ターゲットを中心に回転
	//if (pTargetTransform_)
	//{
	//	Vector3 toTarget = pTargetTransform_->position - pCameraTransform_->position;
	//	center = pTargetTransform_->position + (Vector3::Normalize(-toTarget) * spinDistance_);
	//}
	//// ターゲットがいない場合は正面を向いて回転
	//else
	//{
		center = pCameraTransform_->position + (pCameraTransform_->Forward() * spinDistance_);
	
	// 回転中心からのオフセット
	Vector3 offset = pCameraTransform_->position - center;
	
	// 球面座標系の角度を逆算
	float r = offset.Size();
	if (r > 0.001f) // ゼロ除算を避ける
	{
		// θ (polar angle): 鉛直方向
		spinAngleX_ = acosf(-offset.y / r);
		
		// φ (azimuthal angle): 水平方向
		spinAngleY_ = atan2f(-offset.z, offset.x);
		
		// 距離も更新
		spinDistance_ = r;
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
	, spinDistance_{5.0f}
	, moveSpeed_{10.0f}
	, spinSpeed_{1.0f}
	, rotateSensitivity_{1.0f}
	, thickness_{2.0f}
	, updatingCameraTransform_{ false }
	, cameraOperation_{ CameraOperation::None }
	, followTarget_{true}
	, followDistance_{10.0f}
{
	pCamera_ = new GameObject(
		GameObjectBuilder()
		.SetPosition({ 0,0,0 })
		.SetName("Camera")
		.Build());

	pCameraTransform_ = &Game::System<TransformCP>().Get(pCamera_->GetEntityId());
	hCamera_ = Game::System<CameraSystem>().RegisterDrawCamera(pCameraTransform_);
}

mtgb::ImGuizmoManipulator::~ImGuizmoManipulator()
{
	pCamera_->DestroyMe();
	SAFE_DELETE(pCamera_);
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
		// 左のAltキーを押している場合はターゲットの変更をしない
		if (InputUtil::GetKey(KeyCode::LeftMenu) == false)
		{
			if ((!ImGuizmo::IsViewManipulateHovered() && !ImGuizmo::IsOver()) && IsMouseInWindow(MTImGui::Instance().GetName(ShowType::SceneView)) || !pTargetTransform_)
			{
				SelectTransform();
			}
		}
	}

	DrawTransformGuizmo();

	if (ImGui::Checkbox("AdjustTargetDirection_", &adjustTargetDirection_))
	{

	}
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

	MTImGui::Instance().TypedShow(&pCameraTransform_->position, "cameraPos");
	MTImGui::Instance().TypedShow(&pCameraTransform_->rotate, "cameraRot");
	MTImGui::Instance().DirectShow([this]()
		{
			ImGui::InputFloat4("quat", pCameraTransform_->rotate.f);
		}, "CameraQuat", ShowType::Inspector);
}

void mtgb::ImGuizmoManipulator::FollowTarget()
{
	if (pTargetTransform_)
	{
		if (!followTarget_) return;
		
		if (adjustTargetDirection_)
		{
			pCameraTransform_->rotate = pTargetTransform_->rotate;
			pCameraTransform_->position = pTargetTransform_->position + (pTargetTransform_->Back() * followDistance_);
		}
		else
		{
			SpinCamera(followDistance_);
		}
	}

}

void mtgb::ImGuizmoManipulator::UpdateCamera(const char* _name)
{
	constexpr float ANGLE_SPEED{ DirectX::XMConvertToRadians(100.f) };
	FollowTarget();
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
				InitializeSpinAnglesFromCurrentPosition();

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
		if (mouseMove.Size() != 0)
		{
			// マウス移動量を角度に変換
			spinAngleY_ += mouseMove.x * rotateSensitivity_ * Time::DeltaTimeF(); // 水平角度
			spinAngleX_ += mouseMove.y * rotateSensitivity_ * Time::DeltaTimeF(); // 鉛直角度
			
			// 鉛直角度を制限
			const float MAX_VERTICAL = DirectX::XMConvertToRadians(89.0f);
			spinAngleX_ = std::clamp(spinAngleX_, -MAX_VERTICAL, MAX_VERTICAL);

			static const float ROTATE_DISTANCE = 0.01f;
			// 本当は0.0fを渡してその場で回転させたかったが上手くいかなかった
			SpinCamera(ROTATE_DISTANCE);
		}
	}

	if (cameraOperation_ == CameraOperation::Translate)
	{
		Vector3 mouseMove = InputUtil::GetMouseMove();
		if (mouseMove.Size()  != 0)
		{
			// カメラの右、上ベクトル
			Vector3 right = pCameraTransform_->Right();
			Vector3 up = pCameraTransform_->Up();

			// 移動量を合成
			Vector3 move = right * -mouseMove.x + up * mouseMove.y;

			pCameraTransform_->position += move * moveSpeed_ * Time::DeltaTimeF();
		}
	}

	
	if (cameraOperation_ == CameraOperation::Spin)
	{
		Vector3 mouseMove = InputUtil::GetMouseMove();
		spinAngleX_ += mouseMove.y * spinSpeed_ * Time::DeltaTimeF();
		spinAngleY_ += mouseMove.x * spinSpeed_ * Time::DeltaTimeF();

		SpinCamera(spinDistance_);
	}

}