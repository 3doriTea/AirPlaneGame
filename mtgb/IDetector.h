#pragma once

#include <vector>
#include <string>
#include "WindowContext.h"
#include "RectContainsInfo.h"
#include "GameObjectTag.h"
#include "RectF.h"
namespace mtgb
{
    /// <summary>
    /// 検出システムのインターフェース
    /// </summary>
    class IDetector
    {
    public:
        virtual ~IDetector() = default;

        /// <summary>
        /// 検出を実行
        /// </summary>
        virtual void UpdateDetection() = 0;

        /// <summary>
        /// 検出された対象があるかどうか
        /// </summary>
        /// <returns>対象が一つでもあるなら true</returns>
        virtual bool HasDetectedTargets() const = 0;

        /// <summary>
        /// 検出結果を取得
        /// </summary>
        /// <returns>検出された対象のリスト</returns>
        virtual const std::vector<ScreenCoordContainsInfo>& GetDetectedTargets() const = 0;

        

        /// <summary>
        /// 検出範囲を返す
        /// </summary>
        /// <returns></returns>
        virtual RectF GetDetectionArea() const { return RectF{}; };
        /// <summary>
        /// 検出結果を操作
        /// </summary>
        virtual void ForEach(std::function<void(ScreenCoordContainsInfo&)> _func) = 0;
        virtual void ForEach(std::function<void(const ScreenCoordContainsInfo&)> _func) const = 0;

    protected:
        std::vector<ScreenCoordContainsInfo> detectedTargets_;
    };
}