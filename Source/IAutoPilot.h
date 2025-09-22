#pragma once
#include <mtgb.h>

/// <summary>
/// 自動操縦機能のインタフェース
/// </summary>
class IAutoPilot
{
public:
	IAutoPilot();
	virtual ~IAutoPilot() {};

	/// <summary>
	/// プレイヤーのトランスフォームをセットする
	/// </summary>
	/// <param name="_pTransform">プレイヤーのトランスフォーム</param>
	void SetTransform(Transform* _pTransform) { pTransform_ = _pTransform; }

	/// <summary>
	/// 試しに更新する
	/// </summary>
	/// <returns>オートパイロット作動 true / false</returns>
	virtual bool TryUpdate() = 0;

protected:
	Transform* pTransform_;  // プレイヤーのTransform
};
