#pragma once

#include <vector>
#include <string>
#include <functional>
#include "WindowContext.h"
#include "RectF.h"
#include "RectContainsInfo.h"
namespace mtgb
{
	/// <summary>
	/// 矩形内の対象検出システムの設定
	/// </summary>
	struct RectDetectorConfig
	{
		std::string targetName; // 検出対象の名前
		WindowContext windowContext;	// 対象のウィンドウ
		RectF detectionRect;	// 検出範囲
		
		float maxDistance;		// 判定する最大距離 (farだけじゃなくて nearも設定すべき?)
	};

	struct RectDetector
	{
		RectDetector() = default;
		RectDetector(const RectDetectorConfig& _config);
		RectDetector(RectDetectorConfig&& _config);
		~RectDetector() = default;

		/// <summary>
		/// <para> 検出を実行、結果を取得</para>
		/// <para> 事前に割り当てられた設定を使用</para>
		/// </summary>
		void UpdateDetection();

		/// <summary>
		/// <para> 検出を実行、結果を取得</para>
		/// <para> 引数の設定を使って検出</para>
		/// <param name="_config"> 検出に使用する設定</param>
		void UpdateDetection(RectDetectorConfig& _config);

		/// <summary>
		/// <para> 引数の設定を割り当て、検出を実行</para>
		/// </summary>
		/// <param name="_config"> 割り当て、検出に使用する設定</param>
		void UpdateAndSetDetection(RectDetectorConfig& _config);

		/// <summary>
		/// <para> 引数の設定を割り当て、検出を実行</para>
		/// </summary>
		/// <param name="_config"> 割り当て、検出に使用する設定</param>
		void UpdateAndSetDetection(RectDetectorConfig&& _config);

		/// <summary>
		/// 検出された対象があるかどうか
		/// </summary>
		/// <returns>対象が一つでもあるなら true</returns>
		bool HasDetectedTargets() const;

		void ForEach(std::function<void(RectContainsInfo&)> _func);
		void ForEach(std::function<void(const RectContainsInfo&)> _func) const;
		
		/// <summary>
		/// <para> カメラからターゲットに視線が通っているか</para>
		/// <para> !!!detectedTargets内で総当たりをしているので非効率的。ゲームが重くなったら真っ先に改善!!!</para>
		/// </summary>
		/// <param name="_cameraPos">カメラ</param>
		/// <param name="_targetInfo">ターゲット</param>
		/// <returns></returns>
		bool IsLineOfSight(const Vector3&  _cameraPos, const RectContainsInfo & _targetInfo);

		std::vector<RectContainsInfo> detectedTargets;
		RectDetectorConfig config;
	};
}