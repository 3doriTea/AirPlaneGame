#pragma once

#include "RectDetector.h"
#include "RectF.h"
#include "Handlers.h"
#include "UIDrawCommand.h"
#include "Transform.h"
#include "Vector3.h"

namespace mtgb
{ 
	struct LockOn
	{
		// ロックオンの正方形の一片の長さ
		float lockOnSide;
		float reticleSideExtents;

		RectDetector rectDetector;
		RectContainsInfo* pTargetInfo;

		//RectF searchRect;
		RectF reticleRect;
		bool lockOnAny;
		float lockOnDistance;
		ImageHandle frameImage;
		ImageHandle reticleImage;
		UIParams uiParams;
		Transform* pTransform;
		void Search();
		Vector3 GetTargetPos();
		void Draw() const;
	};
}