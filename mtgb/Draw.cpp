#include "Draw.h"
#include "Figure.h"
#include "ReleaseUtility.h"
#include "Game.h"
#include "Image.h"
#include "Sprite.h"
#include "OBJ.h"
#include "Model.h"
#include "Transform.h"
#include "SceneSystem.h"
#include "DirectWrite.h"

void mtgb::Draw::Box(
	const Vector2Int& _begin,
	const Vector2Int& _end,
	const Color& _color)
{
	Box(RectInt::FromLine(_begin, _end), _color);
}

void mtgb::Draw::Box(const RectInt& _rect, const Color& _color)
{
	Game::System<Draw>().pFigure_->Draw(_rect, _color);
}

void mtgb::Draw::Image(
	const ImageHandle _hImage,
	const RectInt& _draw,
	const RectInt& _cut,
	const float _rotationZ)
{
	Sprite* pSprite{ Game::System<mtgb::Image>().GetSprite(_hImage) };
	pSprite->Draw(_draw, _rotationZ, _cut, Color::WHITE);
}

void mtgb::Draw::Image(
	const ImageHandle _hImage,
	const Transform* _pTransform)
{
	Sprite* pSprite{ Game::System<mtgb::Image>().GetSprite(_hImage) };

	Transform* pCameraTransform = Game::System<SceneSystem>().GetActiveScene()->GetCameraTransform();
	pSprite->Draw(_pTransform, pCameraTransform, pSprite->GetSize(), Color::WHITE);
}

void mtgb::Draw::Model(const ModelHandle _hModel, const Transform* _pTransform)
{
}

void mtgb::Draw::Text(const TextHandle _hText, const Vector2Int& origin)
{
	/*DirectWrite* pDirectWrite*/
	Game::System<mtgb::DirectWrite>().Draw(_hText, static_cast<float>(origin.x), static_cast<float>(origin.y));
}

void mtgb::Draw::Text(const std::string& _text, const Vector2Int& _origin)
{
	Game::System<mtgb::DirectWrite>().ImmediateDraw(_text, static_cast<float>(_origin.x), static_cast<float>(_origin.y));
}

void mtgb::Draw::OBJModel(const OBJModelHandle _hOBJModel, const Transform* _pTransform)
{
	Game::System<mtgb::OBJ>().Draw((int)_hOBJModel, _pTransform);
}

mtgb::Draw::Draw() :
	pFigure_{ nullptr }
{
}

mtgb::Draw::~Draw()
{
	SAFE_DELETE(pFigure_);
}

void mtgb::Draw::Initialize()
{
	pFigure_ = new Figure{};
	pFigure_->Initialize();
}

void mtgb::Draw::Update()
{
}
