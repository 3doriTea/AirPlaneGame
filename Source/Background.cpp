#include "Background.h"

Background::Background() : GameObject(GameObjectBuilder()
	.Build()),
	pAudioPlayer_{ Component<AudioPlayer>() },
	hBGM_{ Audio::Load("Assets\\Sound\\BGM\\playSceneBgm.mp3") }
{
	massert(hBGM_ >= 0 && "BGM‚Ì“Ç‚Ýž‚Ý‚ÉŽ¸”s");

	pAudioPlayer_->SetAudio(hBGM_);
	pAudioPlayer_->SetVolume(0.05f);
	pAudioPlayer_->Play();
}

Background::~Background()
{
}
