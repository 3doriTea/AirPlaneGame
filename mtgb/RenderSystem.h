#pragma once
#include "ISystem.h"
#include "GameScene.h"
#include "ReleaseUtility.h"
#include <functional>  // header ‚Å GameObjectLayerFlag ‚ªŽg‚¦‚È‚¢‚½‚ß

namespace mtgb
{
	class GameObject;

	class RenderSystem : public ISystem
	{
	public:
		void Initialize() override;
		void Update() override;
		void Render(GameScene& _scene);
	private:
		void RenderDirectXWindows(GameScene& _scene);
		void RenderImGuiWindows(GameScene& _scene);
		void RenderGameView(GameScene& _scene);
		void DrawGameObjects(GameScene& _scene, const std::function<bool(GameObject*)> _isDrawTargetCallback);
	};
}
