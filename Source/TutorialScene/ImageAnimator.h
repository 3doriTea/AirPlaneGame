#pragma once
#include <mtgb.h>

/// <summary>
/// 画像をアニメーションさせて描画する
/// </summary>
class ImageAnimator : public mtgb::GameObject
{
public:
	/// <summary>
	/// 表示させる画像の要素
	/// </summary>
	struct Element
	{
		/// <summary>
		/// 1画像
		/// </summary>
		/// <param name="_hImage">画像ハンドル</param>
		/// <param name="_timeSec">表示時間(0以下でデフォルト時間)</param>
		Element(ImageHandle _hImage, float _timeSec = 0.0f) :
			hImage_{ _hImage },
			timeSec_{ _timeSec }
		{}

		ImageHandle hImage_;
		float timeSec_;
	};

	/// <summary>
	/// 表示させる複数の画像要素
	/// </summary>
	class Elements : public std::vector<Element>
	{
	public:
		//using std::vector<Element>::vector;
		Elements(const std::vector<ImageHandle> _hImages)
		{
			for (auto& hImage : _hImages)
			{
				this->push_back(hImage);
			}
		}
	};

	/// <summary>
	/// コンストラクタで渡すアニメーションの設定
	/// </summary>
	struct Setting
	{
		RectF drawRect_;
		float defaultTimeSec_;
		Elements elements_;
		UIParams uIParams_;
	};

public:
	ImageAnimator(const Setting& _setting, const GameObjectLayer _layer);
	~ImageAnimator();

	void Update() override;
	void Draw() const override;

private:
	int showIndex_;
	float timeLeft_;
	Setting setting_;
};
