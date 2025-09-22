#pragma once

#include "Game.h"
#include "cmtgb.h"

#pragma region Utilities

#include "Vector3.h"
#include "Vector2.h"
#include "Vector2T.h"
#include "Vector2F.h"
#include "Vector2Int.h"

#include "RectT.h"
#include "RectInt.h"
#include "RectF.h"
#include "WindowContextUtil.h"

#include "Easing.h"
#include "Mathf.h"
#include "RectDetector.h"

#include "DrawScreenUtility.h"

#pragma endregion

#pragma region Systems

#include "AssetsManager.h"
#include "MainWindow.h"
#include "Screen.h"
#include "ScoreManager.h"
#include "DirectX11Manager.h"
#include "Direct2D/Direct2D.h"
#include "DirectWrite.h"
#include "RigidBodyCP.h"
#include "TransformCP.h"
#include "AudioPlayerCP.h"
#include "ColliderCP.h"
#include "GameTime.h"
#include "Timer.h"
#include "Draw.h"
#include "SceneSystem.h"
#include "RenderSystem.h"
#include "Debug.h"
#include "Image.h"
#include "Audio.h"
#include "OBJ.h"
#include "ImGuiRenderer.h"
#include "Text.h"
#include "Fbx.h"
#include "WindowContext.h"
#include "WindowContextResourceManager.h"
#include "WindowManager.h"
#include "DoubleWindow.h"
#include "CameraSystem.h"
#include "ImGuiShowable.h"
#include "MTImGui.h"
#include "EventManager.h"
#pragma endregion

#include "GameObjectBuilder.h"
#include "GameObject.h"

#pragma region Resource

#include "WindowContextResource.h"
#include "WindowResource.h"
#include "DXGIResource.h"
#include "Direct3DResource.h"
#include "Direct2DResource.h"
#include "InputResource.h"
#include "Input.h"
#include "CameraResource.h"

#pragma endregion
#pragma region Data

#include "InputData.h"
#include "RectContainsInfo.h"
//
#pragma endregion

#pragma region Shaders

#include "IOriginalShader.h"

#pragma endregion

#pragma region Unname

using mtgb::ImageHandle;
using mtgb::AudioHandle;
using mtgb::OBJModelHandle;
using mtgb::FBXModelHandle;

using mtgb::Vector3;
using mtgb::Vector2;
using mtgb::Vector2Int;
using mtgb::Vector2F;
using mtgb::RectInt;
using mtgb::RectF;

using mtgb::GameObject;
using mtgb::Transform;
using mtgb::RigidBody;
using mtgb::AudioPlayer;
using mtgb::Collider;

#pragma endregion
