#include "Image.h"
#include "Sprite.h"
#include "MTStringUtility.h"

mtgb::Image::Image() :
	sprites_{},
	handleCounter_{ 0 }
{
}

mtgb::Image::~Image()
{
	for (auto&& sprite : sprites_)
	{
		delete sprite.second;
	}
	sprites_.clear();
}

void mtgb::Image::Initialize()
{
}

void mtgb::Image::Update()
{
}

mtgb::ImageHandle mtgb::Image::Load(const std::string_view& _fileName)
{
	Image& instance{ Game::System<Image>() };

	Sprite* pSprite{ new Sprite{} };
	pSprite->Initialize();
	pSprite->Load(ToWString(_fileName));
	ImageHandle handle{ ++instance.handleCounter_ };
	instance.sprites_.insert({ handle , pSprite });

	return handle;
}

const mtgb::Vector2Int mtgb::Image::GetSize(const ImageHandle _imageHandle)
{
	return Game::System<Image>().GetSprite(_imageHandle)->GetSize();
}
