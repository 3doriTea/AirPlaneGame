#pragma once

#include "IDetector.h"
#include "DetectorConfigs.h"
#include <functional>

namespace mtgb
{
    /// <summary>
    /// レイ方向・角度による対象検出システム
    /// </summary>
    class RayDetector : public IDetector
    {
    public:
        RayDetector() = default;
        RayDetector(const RayDetectorConfig& _config);
        RayDetector(RayDetectorConfig&& _config);
        ~RayDetector() = default;

        /// <summary>
        /// 検出を実行
        /// </summary>
        void UpdateDetection() override;

        /// <summary>
        /// 検出を実行（設定指定）
        /// </summary>
        void UpdateDetection(const RayDetectorConfig& _config);

        /// <summary>
        /// 設定を更新して検出を実行
        /// </summary>
        void UpdateAndSetDetection(RayDetectorConfig&& _config);

        /// <summary>
        /// 検出された対象があるかどうか
        /// </summary>
        bool HasDetectedTargets() const override;

        /// <summary>
        /// 検出結果を取得
        /// </summary>
        const std::vector<ScreenCoordContainsInfo>& GetDetectedTargets() const override;

        void ForEach(std::function<void(ScreenCoordContainsInfo&)> _func) override;
        void ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const override;

        /// <summary>
        /// ターゲットがレイの角度範囲内にあるかチェック
        /// </summary>
        bool IsTargetInRayAngle(const Vector3& targetPos, const Vector3& rayOrigin, 
                               const Vector3& rayDirection, float maxAngleDegrees) const;

        RayDetectorConfig config;
    };
}