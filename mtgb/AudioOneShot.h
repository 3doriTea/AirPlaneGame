#pragma once

struct IXAudio2SourceVoice;

namespace mtgb
{
	class AudioClip;

	struct AudioOneShot
	{
		AudioOneShot();
		~AudioOneShot();

		float timeLeft;
		IXAudio2SourceVoice* pSourceVoice;
		AudioClip* pAudioClip;
	};
}
