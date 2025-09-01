#pragma once

struct IXAudio2SourceVoice;

namespace mtgb
{
	class AudioClip;

	struct AudioOneShot
	{
		AudioOneShot();
		~AudioOneShot();

		void Release();

		float timeLeft;
		IXAudio2SourceVoice* pSourceVoice;
		AudioClip* pAudioClip;
	};
}
