#pragma once
#include <mtgb.h>

class Background : public GameObject
{
public:
	Background();
	~Background();

private:
	AudioHandle hBGM_;
	AudioPlayer* pAudioPlayer_;
};
