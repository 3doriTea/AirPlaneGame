#pragma once
#include "ISystem.h"
#include "Vector2Int.h"
#include "Vector2F.h"

namespace mtgb
{
	class Screen : public ISystem
	{
	public:
		Screen();
		~Screen();

		void Initialize() override;
		void Update() override;

		inline const Vector2Int GetSize() const { return Vector2Int{ width_,height_ }; }
		inline const Vector2F GetSizeF() const { return Vector2F{ static_cast<float>(width_),static_cast<float>(height_) }; }
		inline const Vector2Int GetInitialSize() const { return Vector2Int{ initialWidth_,initialHeight_ }; }
		/// <summary>
		/// 初期値を1としたときのスクリーンサイズの比率を返す
		/// </summary>
		/// <returns></returns>
		inline const Vector2F GetSizeRatio() const
		{
			return Vector2F
			{
				static_cast<float>(width_) / static_cast<float>(initialWidth_),
				static_cast<float>(height_) / static_cast<float>(initialHeight_)
			};
		}

		void SetSize(int _width, int _height);

	private:

		int initialWidth_; // スクリーンの横幅(初期値)
		int initialHeight_; // スクリーンの縦幅(初期値)
		int width_;     // スクリーンの横幅
		int height_;    // スクリーンの縦幅
		int fpsLimit_;  // FPS制限 (1フレームあたりの更新フレーム数)
	};
}
