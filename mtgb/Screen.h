#pragma once
#include "ISystem.h"
#include "Vector2Int.h"

namespace mtgb
{
	class Screen : public ISystem
	{
	public:
		Screen();
		~Screen();

		void Initialize() override;
		void Update() override;

		inline const Vector2Int GetSize() const { return{ width_, height_ }; }

	private:
		int width_;     // スクリーンの横幅
		int height_;    // スクリーンの縦幅
		int fpsLimit_;  // FPS制限 (1フレームあたりの更新フレーム数)
	};
}
