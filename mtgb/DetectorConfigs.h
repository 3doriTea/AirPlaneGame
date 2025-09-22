#pragma once

#include "WindowContext.h"
#include "RectF.h"
#include "Vector2F.h"
#include "Vector3.h"
#include "GameObjectTag.h"
#include "Transform.h"
#include "UIDrawCommand.h"
namespace mtgb
{
    /// <summary>
    /// 検出器の共通設定
    /// </summary>
    struct DetectorConfigBase
    {
        //std::string targetName;         // 検出対象の名前
        GameObjectTag targetTag;        // 検出対象のタグ
        WindowContext windowContext;    // 対象のウィンドウ
        float maxDistance;              // 判定する最大距離
        float minDistance = 0.0f;       // 判定する最小距離
        UIParams uiParams;

    };

    /// <summary>
    /// 矩形検出器の設定
    /// </summary>
    struct RectDetectorConfig
    {
        DetectorConfigBase base;
        RectF detectionRect;            // 検出範囲の矩形
    };

    /// <summary>
    /// 円検出器の設定
    /// </summary>
    struct CircleDetectorConfig
    {
        DetectorConfigBase base;
        Vector2F center;                // 円の中心座標（スクリーン座標）
        float radius;                   // 円の半径
    };

    /// <summary>
    /// レイ検出器の設定
    /// </summary>
    struct RayDetectorConfig
    {
        DetectorConfigBase base;
        Transform* rayTransform;           // レイの起点、方向
        float maxAngleDegrees;            // 検出可能な最大角度（度）
    };
}