// TargetingSystemの使用例

#include "TargetingSystem.h"

void ExampleUsage()
{
    TargetingSystem targetingSystem;
    Transform* myTransform = /* プレイヤーのTransform */;

    // 1. 矩形検出として初期化
    Vector2F screenCenter = { 400.0f, 300.0f };
    float detectionSize = 200.0f;
    targetingSystem.InitializeAsRect(myTransform, screenCenter, detectionSize);

    // 2. 円形検出に切り替え
    float radius = 100.0f;
    targetingSystem.InitializeAsCircle(myTransform, screenCenter, radius);

    // 3. レイ検出に切り替え
    Vector3 forwardDirection = myTransform->Forward();
    float maxAngle = 45.0f; // 45度の角度範囲
    float maxDistance = 100.0f;
    targetingSystem.InitializeAsRay(myTransform, forwardDirection, maxAngle, maxDistance);

    // ターゲット検索
    targetingSystem.SearchTargets();

    // ターゲットがあれば射撃
    if (targetingSystem.HasTarget())
    {
        targetingSystem.FireAtTarget();
    }

    // UI描画
    targetingSystem.DrawUI();
}