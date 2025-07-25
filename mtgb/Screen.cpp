#include "Screen.h"
#include <string>
#include "ProfileUtlity.h"

mtgb::Screen::Screen() :
	width_{ 0 },
	height_{ 0 },
	fpsLimit_{ 0 }
{
}

mtgb::Screen::~Screen()
{
}

void mtgb::Screen::Initialize()
{
	// �X�N���[���̏����擾
	width_    = ProfileInt::Load().Section("SCREEN").Param("Width") .InitValue(800).Get();
	height_   = ProfileInt::Load().Section("SCREEN").Param("Height").InitValue(600).Get();
	fpsLimit_ = ProfileInt::Load().Section("GAME")  .Param("Fps")   .InitValue(60) .Get();
}

void mtgb::Screen::Update()
{
}
