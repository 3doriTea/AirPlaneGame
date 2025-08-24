#pragma once
#include <mtgb.h>
#include "ShaderCommon.h"


class WingEffect :
	public game::IOriginalShader<WingEffect>,
	public game::IOriginalShader<WingEffect>::IWriteVertex,
	public game::IOriginalShader<WingEffect>::IWriteIndex,
	public game::IOriginalShader<WingEffect>::IWriteConstant
{
public:
	struct Vertex
	{
		Vector3 position;
	};

	struct ConstantBuffer
	{
		Matrix4x4 g_matrixCameraTranslate;          // カメラ座標変換行列
		Matrix4x4 g_matrixCameraRotation;          // カメラ座標変換行列
		Matrix4x4 g_matrixWorldTranslate;  // 頂点座標変換行列
		Matrix4x4 g_matrixWorldRotation;   // 頂点座標変換行列
		Matrix4x4 g_matrixTexture;         // テクスチャ座標変換行列
		Vector4 g_color;                   // 色
		Vector2 g_angle;                   // 角度
	};

	WingEffect() {}
	~WingEffect() {}


};

