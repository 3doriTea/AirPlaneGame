#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <string>
#include <map>
#include "Vector2Int.h"

namespace mtgb
{
	class Sprite;

	class Image : public ISystem
	{
	public:
		Image();
		~Image();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// 画像ハンドルからスプライトのポインタを取得
		/// </summary>
		/// <param name="_imageHandle"></param>
		/// <returns></returns>
		inline Sprite* GetSprite(const ImageHandle _imageHandle)
		{
			massert((0 < _imageHandle) && (_imageHandle <= Image::handleCounter_)
				&& "無効なハンドラ @Image::GetSprite");

			massert(Image::sprites_.count(_imageHandle) != 0
				&& "スプライトが存在しない @Image::GetSprite");

			return Image::sprites_[_imageHandle];
		}
	//private:
		
		/// <summary>
		/// 画像を読み込み、ハンドラを取得
		/// </summary>
		/// <param name="_fileName">ファイルパス</param>
		/// <returns>画像のハンドル</returns>
		static ImageHandle Load(const std::string_view& _fileName);
		/// <summary>
		/// 読み込んだ画像のサイズを取得
		/// </summary>
		/// <param name="_imageHandle">画像のハンドル</param>
		/// <returns>画像の縦横サイズ</returns>
		static const Vector2Int GetSize(const ImageHandle _imageHandle);

	private:
		ImageHandle handleCounter_;               // ハンドラの加算用
		std::map<ImageHandle, Sprite*> sprites_;  // 読み込まれたスプライト
	};
}
