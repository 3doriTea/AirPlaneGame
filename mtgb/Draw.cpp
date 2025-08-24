#include "Draw.h"
#include "ReleaseUtility.h"
#include "Game.h"
#include "Image.h"
#include "Sprite.h"
#include "OBJ.h"
#include "Fbx.h"
#include "FbxParts.h"
#include "Text.h"
#include "Model.h"
#include "Transform.h"
#include "CameraSystem.h"
#include "DirectWrite.h"
#include "MTAssert.h"
#include "ImGuiRenderer.h"
#include "MTStringUtility.h"
#include <dwrite.h>
#include "Ground.h"
#include "Figure.h"


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
	const RectF& _draw,
	const RectF& _cut,
	const float _rotationZ,
	const UIParams& _uiParams)
{
	uiDrawCommands_.insert({
		_uiParams,
		[=]()
		{
			CheckSetShader(ShaderType::Sprite2D);
			Sprite* pSprite{ Game::System<mtgb::Image>().GetSprite(_hImage) };
			pSprite->Draw(_draw, _rotationZ, _cut, Color::WHITE);
		}
		});
}

void mtgb::Draw::Image(const ImageHandle _hImage, const RectF& _draw, const UIParams& _uiParams)
{
	Image(_hImage, _draw, { Vector2F::Zero(), Image::GetSizeF(_hImage) }, 0.0f, _uiParams);
}

void mtgb::Draw::Image(
	const ImageHandle _hImage,
	const Transform& _transform,
	const UIParams& _uiParams)
{
	uiDrawCommands_.insert({
		_uiParams,
		[=]() {
			CheckSetShader(ShaderType::Sprite2D);

	Sprite* pSprite{ Game::System<mtgb::Image>().GetSprite(_hImage) };

	const Transform* pCameraTransform = &(Game::System<CameraSystem>().GetTransform());
	pSprite->Draw(&_transform, pCameraTransform, pSprite->GetSize(), Color::WHITE);
		} });
	
}

void mtgb::Draw::Image(const ImageHandle _hImage, Transform&& _transform, const UIParams& _uiParams)
{

	uiDrawCommands_.insert({
	_uiParams,
	[=,transform = std::move(_transform)]() mutable {
		CheckSetShader(ShaderType::Sprite2D);

	Sprite* pSprite{ Game::System<mtgb::Image>().GetSprite(_hImage) };

	const Transform* pCameraTransform = &(Game::System<CameraSystem>().GetTransform());
	pSprite->Draw(&transform, pCameraTransform, pSprite->GetSize(), Color::WHITE);
	} });

}

void mtgb::Draw::Model(const ModelHandle _hModel, const Transform* _pTransform)
{
	// TODO: FbxとObjをモデルとしてハンドル含め統合、自動分岐する
	massert(false && "Draw::Modelが呼ばれていますが未実装です。FbxとObjで別関数を呼んでください。 @Draw::Model");
}

void mtgb::Draw::Text(const TextHandle _hText, const Vector2F& _origin,TextAlignment _alignment, const UIParams& _uiParams)
{
	uiDrawCommands_.insert({
	_uiParams,
	[=]() {
		DirectX11Draw::SetIsWriteToDepthBuffer(false);
		CheckSetShader(ShaderType::Sprite2D);

		TextLayoutData* layoutData = Game::System<mtgb::Text>().GetTextLayoutData(_hText);
		auto formatData = Game::System<mtgb::Text>().GetOrCreateTextFormat(layoutData->fontSize);

		Game::System<mtgb::DirectWrite>().SetTextAlignment(_alignment, layoutData->layout);
		Game::System<mtgb::DirectWrite>().Draw(layoutData->layout, _origin.x, _origin.y + formatData.second.textTopOffset);
		} });
}

void mtgb::Draw::ImmediateText(const std::string& _text, Vector2F _topLeft, int _size, TextAlignment _alignment,  const UIParams& _uiParams)
{
	Vector2Int layoutBoxSize = Game::System<Screen>().GetSize();
	ImmediateText(_text, { _topLeft.x,_topLeft.y,static_cast<float>(layoutBoxSize.x), static_cast<float>(layoutBoxSize.y) }, _size, _alignment, _uiParams);
}

void mtgb::Draw::ImmediateText(std::string&& _text, Vector2F _topLeft, int _size, TextAlignment _alignment, const UIParams& _uiParams)
{
	Vector2Int layoutBoxSize = Game::System<Screen>().GetSize();
	ImmediateText(std::move(_text), { _topLeft.x,_topLeft.y,static_cast<float>(layoutBoxSize.x), static_cast<float>(layoutBoxSize.y) }, _size, _alignment,_uiParams);
}

void mtgb::Draw::ImmediateText(const std::string& _text, RectF _rect, int _size, TextAlignment _alignment, const UIParams& _uiParams)
{
	uiDrawCommands_.insert({
		_uiParams,
		[=]() {
			DirectX11Draw::SetIsWriteToDepthBuffer(false);
			CheckSetShader(ShaderType::Sprite2D);

			auto formatData = Game::System<mtgb::Text>().GetOrCreateTextFormat(_size);
			Game::System<DirectWrite>().SetTextAlignment(_alignment, formatData.first);
			Game::System<DirectWrite>().ImmediateDraw(ToWString(_text), formatData.first, formatData.second, 
				_rect.x,
				_rect.y,
				_rect.width,
				_rect.height);
		}
		});
}

void mtgb::Draw::ImmediateText(std::string&& _text, RectF _rect, int _size, TextAlignment _alignment, const UIParams& _uiParams)
{

	uiDrawCommands_.insert({
	_uiParams,
	[=, text = std::move(_text)]() mutable {
		DirectX11Draw::SetIsWriteToDepthBuffer(false);
		CheckSetShader(ShaderType::Sprite2D);

		auto formatData = Game::System<mtgb::Text>().GetOrCreateTextFormat(_size);
		Game::System<DirectWrite>().SetTextAlignment(_alignment, formatData.first);
		Game::System<DirectWrite>().ImmediateDraw(ToWString(text), formatData.first, formatData.second,
			_rect.x,
			_rect.y,
			_rect.width,
			_rect.height);
	}
		});
}



void mtgb::Draw::ChangeFontSize(int _size)
{
	currentDefaultFontSize_ = _size;
	auto fontFormatData = Game::System<mtgb::Text>().GetOrCreateTextFormat(_size);
	Game::System<DirectWrite>().ChangeFormat(fontFormatData.first, fontFormatData.second);
}

void mtgb::Draw::ChangeTextAlignment(TextAlignment _alignment)
{
	currentDefaultTextAlignment_ = _alignment;
}

void mtgb::Draw::GroundPlane()
{
	DirectX11Draw::SetIsWriteToDepthBuffer(true);
	CheckSetShader(ShaderType::Ground);

	Game::System<Draw>().pGround_->Draw();
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
	pFigure_{ nullptr },
	pGround_{ nullptr },
	pFbxModel_{ nullptr }
{
}

mtgb::Draw::~Draw()
{
	SAFE_DELETE(pFigure_);
	SAFE_RELEASE(pFbxModel_);
	SAFE_DELETE(pGround_);
}

void mtgb::Draw::Initialize()
{
	pFigure_ = new Figure{};
	pFigure_->Initialize();

	pFbxModel_ = new FbxModel{};
	pFbxModel_->Load("Model/GroundPlane.fbx");
	FbxParts* pParts{ pFbxModel_->GetFbxParts(0) };

	pGround_ = new Ground{ pParts->GetNode() };
	pGround_->Initialize();
}

void mtgb::Draw::Update()
{
}

void mtgb::Draw::FlushUIDrawCommands(GameObjectLayer _layer)
{
	using mtbit::operator|;
	for (auto& drawCommand : uiDrawCommands_)
	{
		if (drawCommand.params.layerFlag.Has(GameObjectLayer::All | _layer))
		{
			drawCommand.drawFunction();
		}
	}
}

void mtgb::Draw::ClearUICommands()
{
	uiDrawCommands_.clear();
}

ShaderType mtgb::Draw::onceShaderType_{ ShaderType::Max };
int mtgb::Draw::currentDefaultFontSize_{ 36 };
TextAlignment mtgb::Draw::currentDefaultTextAlignment_{ TextAlignment::center };
mtgb::UIParams mtgb::Draw::defaultUIParams_{};
std::multiset<mtgb::UIDrawCommand> mtgb::Draw::uiDrawCommands_{};