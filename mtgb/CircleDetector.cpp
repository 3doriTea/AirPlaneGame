#include "CircleDetector.h"
#include "CameraSystem.h"
#include "Game.h"
#include "GameObject.h"
#include <cmath>

namespace mtgb
{
    CircleDetector::CircleDetector(const CircleDetectorConfig& _config)
        : config(_config)
    {
    }

    CircleDetector::CircleDetector(CircleDetectorConfig&& _config)
        : config(std::move(_config))
    {
    }

    void CircleDetector::UpdateDetection()
    {
        UpdateDetection(config);
    }

    void CircleDetector::UpdateDetection(const CircleDetectorConfig& _config)
    {
        detectedTargets_.clear();

        // タグで取得
        std::vector<GameObject*> findObjs;
        GameObject::FindGameObjects(_config.targetTag, &findObjs);

        for (const auto& obj : findObjs)
        {
            
            // Transform取得
            Transform* pTransform = &Transform::Get(obj->GetEntityId());
            Vector3 worldPos = pTransform->GetWorldPosition();

            // 距離チェック
            // カメラ位置を取得
            Vector3 cameraPos = Game::System<CameraSystem>().GetTransform(_config.windowContext).GetWorldPosition();
            float distance = (worldPos - cameraPos).Size();
            
            if (distance < _config.minDistance || distance > _config.maxDistance)
            {
                continue;
            }

            // ワールド座標をスクリーン座標に変換
            Vector3 screenPos = Game::System<CameraSystem>().GetWorldToScreenPos(worldPos, _config.windowContext);
            
            // スクリーン座標が有効範囲内かチェック
            if (screenPos.z < 0.0f || screenPos.z > 1.0f)
            {
                continue;
            }

            // 円形範囲内かチェック
            Vector2F screenPos2D = { screenPos.x, screenPos.y };
            if (IsPointInCircle(screenPos2D, _config.center, _config.radius))
            {
                detectedTargets_.emplace_back(worldPos, screenPos, obj->GetEntityId());
            }
        }
    }

    void CircleDetector::UpdateAndSetDetection(CircleDetectorConfig&& _config)
    {
        config = std::move(_config);
        UpdateDetection();
    }

    bool CircleDetector::HasDetectedTargets() const
    {
        return !detectedTargets_.empty();
    }

    const std::vector<ScreenCoordContainsInfo>& CircleDetector::GetDetectedTargets() const
    {
        return detectedTargets_;
    }

    void CircleDetector::ForEach(std::function<void(ScreenCoordContainsInfo&)> _func)
    {
        for (auto& target : detectedTargets_)
        {
            _func(target);
        }
    }

    void CircleDetector::ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const
    {
        for (const auto& target : detectedTargets_)
        {
            _func(target);
        }
    }

    bool CircleDetector::IsPointInCircle(const Vector2F& point, const Vector2F& center, float radius) const
    {
        float dx = point.x - center.x;
        float dy = point.y - center.y;
        float distanceSquared = dx * dx + dy * dy;
        return distanceSquared <= (radius * radius);
    }
}