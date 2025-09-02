#include "Screen.h"
#include <string>
#include "ProfileUtlity.h"
#include "WindowManager.h"
#include "Game.h"
#include "WindowContext.h"
mtgb::Screen::Screen() :
	width_{ 0 },
	height_{ 0 },
	sizeRatio_{1.0f,1.0f},
	fpsLimit_{ 0 }
{
}

mtgb::Screen::~Screen()
{
}

void mtgb::Screen::Initialize()
{
	// ÉXÉNÉäÅ[ÉìÇÃèÓïÒÇéÊìæ
	initialWidth_    = ProfileInt::Load().Section("SCREEN").Param("Width") .InitValue(800).Get();
	initialHeight_   = ProfileInt::Load().Section("SCREEN").Param("Height").InitValue(600).Get();
	fpsLimit_ = ProfileInt::Load().Section("GAME")  .Param("Fps")   .InitValue(60) .Get();

	width_ = initialWidth_;
	height_ = initialHeight_;
}

void mtgb::Screen::Update()
{
}


const mtgb::Vector2F mtgb::Screen::GetSizeRatio() const
{
	return sizeRatio_;
}

void mtgb::Screen::SetSize(int _width, int _height)
{
	width_ = _width;
	height_ = _height;

	
	sizeRatio_.x = static_cast<float>(width_) / initialWidth_;
	sizeRatio_.y = static_cast<float>(height_) / initialHeight_;
}
