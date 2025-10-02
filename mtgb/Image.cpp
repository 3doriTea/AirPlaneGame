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

	for (auto itr = instance.sprites_.begin(); itr != instance.sprites_.end(); itr++)
	{
		std::wstring str{ _fileName.begin(), _fileName.end() };
		if ((*itr).second->GetFileName() == str)
		{
			return (*itr).first;
		}
	}

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

const mtgb::Vector2F mtgb::Image::GetSizeF(const ImageHandle _imageHandle)
{
	mtgb::Vector2Int size = Game::System<Image>().GetSprite(_imageHandle)->GetSize();
	Vector2F ret = mtgb::Vector2F(size.x, size.y);
	return ret;
	//return mtgb::Vector2F(static_cast<float>(size.x), static_cast<float>(size.y));

}
