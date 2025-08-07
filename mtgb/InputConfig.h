#pragma once
#include "Axis.h"
typedef long LONG;
namespace mtgb
{
	struct InputConfig
	{

		LONG xRange;//x軸の値の範囲
		LONG yRange;//y軸の値の範囲
		LONG zRange;//z軸の値の範囲

		float deadZone;//入力を無視する閾値

		/// <summary>
		/// x,y,z軸すべてに範囲の設定
		/// 下限が負、上限が正
		/// </summary>
		/// <param name="_range">範囲となる絶対値</param>
		void SetRange(LONG _range);

		/// <summary>
		/// 指定した軸に範囲の設定
		/// 下限が負、上限が正
		/// </summary>
		/// <param name="_range">範囲となる絶対値</param>
		/// <param name="_axis">設定する軸</param>
		void SetRange(LONG _range, Axis _axis);

		/// <summary>
		/// 指定した値(割合)でデッドゾーンを設定
		/// </summary>
		/// <param name="_deadZone">0～1の範囲で指定してください</param>
		void SetDeadZone(float _deadZone);


		/// <summary>
		/// デッドゾーンを適用する
		/// </summary>
		/// <param name="value">適用する値</param>
		/// <returns>適用された値</returns>
		float ApplyDeadZone(const float value) const;
	};
}