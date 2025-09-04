#include "AudioOneShot.h"
#include <xaudio2.h>
#include "AudioClip.h"
#include "ReleaseUtility.h"

mtgb::AudioOneShot::AudioOneShot() :
	timeLeft{ 0 },
	pSourceVoice{ nullptr },
	pAudioClip{ new AudioClip{} }
{
}

mtgb::AudioOneShot::~AudioOneShot()
{
	
}

void mtgb::AudioOneShot::Release()
{
	if (pSourceVoice != nullptr)
	{
		pSourceVoice->DestroyVoice();
		pSourceVoice = nullptr;
	}

	SAFE_DELETE(pAudioClip);
}
