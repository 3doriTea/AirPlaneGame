#include "Draw.h"
#include "Figure.h"
#include "ReleaseUtility.h"
#include "Game.h"
#include "Image.h"
#include "Sprite.h"
#include "OBJ.h"
#include "Fbx.h"
#include "Text.h"
#include "Model.h"
#include "Transform.h"
#include "CameraSystem.h"
#include "DirectWrite.h"

void mtgb::Draw::CheckSetShader(const ShaderType _default)
{
	if (onceShaderType_ == ShaderType::Max)
	{
		// シェーダがセットされていないなら既定シェーダ
		DirectX11Draw::SetShader(_default);
	}
	else
	{
		// シェーダがセットされているなら優先
		DirectX11Draw::SetShader(onceShaderType_);
		onceShaderType_ = ShaderType::Max;  // 一度使ったら外す
	}
}

void mtgb::Draw::Box(
	const Vector2Int& _begin,
	const Vector2Int& _end,
	const Color& _color)
{
	CheckSetShader(ShaderType::Figure);

	Box(RectInt::FromLine(_begin, _end), _color);
}

void mtgb::Draw::Box(const RectInt& _rect, const Color& _color)
{
	CheckSetShader(ShaderType::Figure);

	Game::System<Draw>().pFigure_->Draw(_rect, _color);
}

void mtgb::Draw::Image(
	const ImageHandle _hImage,
	const RectInt& _draw,
	const RectInt& _cut,
	const float _rotationZ)
{
	CheckSetShader(ShaderType::Sprite2D);

	Sprite* pSprite{ Game::System<mtgb::Image>().GetSprite(_hImage) };
	pSprite->Draw(_draw, _rotationZ, _cut, Color::WHITE);
}

void mtgb::Draw::Image(
	const ImageHandle _hImage,
	const Transform* _pTransform)
{
	CheckSetShader(ShaderType::Sprite2D);

	Sprite* pSprite{ Game::System<mtgb::Image>().GetSprite(_hImage) };

	const Transform* pCameraTransform = &(Game::System<CameraSystem>().GetTransform());
	pSprite->Draw(_pTransform, pCameraTransform, pSprite->GetSize(), Color::WHITE);
}

void mtgb::Draw::Model(const ModelHandle _hModel, const Transform* _pTransform)
{
}

void mtgb::Draw::Text(const TextHandle _hText, const Vector2Int& origin)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	CheckSetShader(ShaderType::Sprite2D);
	
	Game::System<mtgb::Text>().Draw(_hText, static_cast<float>(origin.x), static_cast<float>(origin.y));
}

void mtgb::Draw::ImmediateText(const std::string& text, float x, float y)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	CheckSetShader(ShaderType::Sprite2D);

	Game::System<mtgb::Text>().ImmediateDraw(text, x, y);
}

void mtgb::Draw::ImmediateText(const std::string& text, float x, float y, int size)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	CheckSetShader(ShaderType::Sprite2D);

	Game::System<mtgb::Text>().ImmediateDraw(text, x, y, size);
}

void mtgb::Draw::OBJModel(const OBJModelHandle _hOBJModel, const Transform* _pTransform)
{
	CheckSetShader(ShaderType::FbxParts);

	Game::System<mtgb::OBJ>().Draw((int)_hOBJModel, _pTransform);
}

void mtgb::Draw::FBXModel(const FBXModelHandle _hFBXModel, const Transform& _pTransform, const int _frame)
{
	CheckSetShader(ShaderType::FbxParts);

	Game::System<mtgb::Fbx>().Draw(_hFBXModel, _pTransform, _frame);
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

ShaderType mtgb::Draw::onceShaderType_{ ShaderType::Max };
