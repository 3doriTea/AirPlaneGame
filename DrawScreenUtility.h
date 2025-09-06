#pragma once
#include "RectF.h"

namespace mtgb
{
	/// <summary>
	/// 設計上のスクリーン矩形から描画時のスクリーン矩形を生成する
	/// </summary>
	/// <param name="_rect">設計時のスクリーン矩形</param>
	/// <returns>描画時のスクリーン矩形</returns>
	static const RectF GenDrawScreenFrom(const RectF& _rect);
}
