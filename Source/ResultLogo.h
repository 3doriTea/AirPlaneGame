#pragma once
#include <mtgb.h>

class ResultLogo : public GameObject
{
public:
	ResultLogo();
	ResultLogo(mtgb::WindowContext _context);
	~ResultLogo();

	void Update() override;
	void Draw() const override;

private:
	//FBXModelHandle fModel_;
	ImageHandle hImage_;  // リザルト文字の画像ハンドル
	Transform* pTransform_;
	mtgb::WindowContext context_;

};