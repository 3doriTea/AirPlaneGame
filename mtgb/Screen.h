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

		inline const Vector2Int GetSize() const { return size_; }
		inline const Vector2F GetSizeF() const { return Vector2F{ static_cast<float>(size_.x),static_cast<float>(size_.y) }; }
		//inline const Vector2Int GetInitialSize() const { return Vector2Int{ initialWidth_,initialHeight_ }; }
		/// <summary>
		/// 初期値を1としたときのスクリーンサイズの比率を返す
		/// </summary>
		/// <returns></returns>
		const Vector2F GetSizeRatio() const;
		const mtgb::Vector2Int GetInitialSize() const;

		

		/// <summary>
		/// <para> スクリーンのサイズを保持する</para>
		/// </summary>
		/// <param name="_width"></param>
		/// <param name="_height"></param>
		void SetSize(int _width, int _height);

	private:
		Vector2F sizeRatio_;
		Vector2Int initialSize_;
		Vector2Int size_;
		//int initialWidth_; // スクリーンの横幅(初期値)
		//int initialHeight_; // スクリーンの縦幅(初期値)
		//int width_;     // スクリーンの横幅
		//int height_;    // スクリーンの縦幅
		int fpsLimit_;  // FPS制限 (1フレームあたりの更新フレーム数)
	};
}
