#include "RayDetector.h"
#include "CameraSystem.h"
#include "Game.h"
#include "GameObject.h"
#include "Vector3.h"
#include <cmath>
#include <DirectXMath.h>
#include <algorithm>
namespace mtgb
{
    RayDetector::RayDetector(const RayDetectorConfig& _config)
        : config(_config)
    {
    }

    RayDetector::RayDetector(RayDetectorConfig&& _config)
        : config(std::move(_config))
    {
    }

    void RayDetector::UpdateDetection()
    {
        UpdateDetection(config);
    }

    void RayDetector::UpdateDetection(const RayDetectorConfig& _config)
    {
        detectedTargets_.clear();

        
        // タグで取得
        std::vector<GameObject*> findObjs;
        GameObject::FindGameObjects(_config.base.targetTag,&findObjs);

        for (const auto& obj : findObjs)
        {
            // Transform取得
            Transform* pTransform = &Transform::Get(obj->GetEntityId());
            Vector3 worldPos = pTransform->GetWorldPosition();
            Vector3 rayOrigin = _config.rayTransform->GetWorldPosition();
            Vector3 rayDirection = _config.rayTransform->Forward();

            // 距離チェック
            float distance = (worldPos - rayOrigin).Size();
            
            if (distance < _config.base.minDistance || distance > _config.base.maxDistance)
            {
                continue;
            }

            // レイの角度範囲内かチェック
            if (IsTargetInRayAngle(worldPos, rayOrigin, rayDirection, _config.maxAngleDegrees))
            {
                // ワールド座標をスクリーン座標に変換
                Vector3 screenPos = Game::System<CameraSystem>().GetWorldToScreenPos(worldPos, _config.base.windowContext);
                detectedTargets_.emplace_back(worldPos, screenPos, obj->GetEntityId());
            }
        }
    }

    void RayDetector::UpdateAndSetDetection(RayDetectorConfig&& _config)
    {
        config = std::move(_config);
        UpdateDetection();
    }

    bool RayDetector::HasDetectedTargets() const
    {
        return !detectedTargets_.empty();
    }

    const std::vector<ScreenCoordContainsInfo>& RayDetector::GetDetectedTargets() const
    {
        return detectedTargets_;
    }

    void RayDetector::ForEach(std::function<void(ScreenCoordContainsInfo&)> _func)
    {
        for (auto& target : detectedTargets_)
        {
            _func(target);
        }
    }

    void RayDetector::ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const
    {
        for (const auto& target : detectedTargets_)
        {
            _func(target);
        }
    }

    bool RayDetector::IsTargetInRayAngle(const Vector3& targetPos, const Vector3& rayOrigin, 
                                        const Vector3& rayDirection, float maxAngleDegrees) const
    {
        // レイの起点からターゲットへのベクトル
        Vector3 toTarget = Vector3::Normalize(targetPos - rayOrigin);
        
        // レイの方向ベクトルを正規化
        Vector3 normalizedRayDir = Vector3::Normalize(rayDirection);
        
        // 内積を使って角度を計算
        float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(normalizedRayDir, toTarget));
        
        // 内積から角度を計算（ラジアン）
        float angleRadians = std::acosf(std::clamp(dotProduct, -1.0f, 1.0f));
        
        // 度に変換
        float angleDegrees = DirectX::XMConvertToDegrees(angleRadians);
        
        // 指定角度以内かチェック
        return angleDegrees <= maxAngleDegrees;
    }
}