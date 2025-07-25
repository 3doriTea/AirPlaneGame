#pragma once

#include "Game.h"
#include "cmtgb.h"

#pragma region Utilities

#include "Vector3.h"
#include "Vector2.h"
#include "Vector2Int.h"
#include "RectInt.h"

#pragma endregion

#pragma region Systems

#include "AssetsManager.h"
#include "MainWindow.h"
#include "Screen.h"
#include "DirectX11Manager.h"
#include "Direct2D/Direct2D.h"
#include "DirectWrite.h"
#include "RigidBodyCP.h"
#include "TransformCP.h"
#include "AudioPlayerCP.h"
#include "GameTime.h"
#include "Timer.h"
#include "Draw.h"
#include "SceneSystem.h"
#include "Debug.h"
#include "Image.h"
#include "Audio.h"
#include "OBJ.h"
#include "MTImGui.h"

#pragma endregion

#include "GameObjectBuilder.h"
#include "GameObject.h"

#pragma region Data

#include "InputData.h"

#pragma endregion

#pragma region Unname

using mtgb::ImageHandle;
using mtgb::AudioHandle;
using mtgb::OBJModelHandle;

using mtgb::Vector3;
using mtgb::Vector2;
using mtgb::Vector2Int;
using mtgb::RectInt;

using mtgb::GameObject;
using mtgb::Transform;
using mtgb::RigidBody;
using mtgb::AudioPlayer;

#pragma endregion
