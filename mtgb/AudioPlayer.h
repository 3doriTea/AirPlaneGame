#pragma once
#include "IComponent.h"
#include "AudioPlayerCP.h"
#include "Handlers.h"

struct IXAudio2SourceVoice;

namespace mtgb
{
	class AudioClip;
	class AudioPlayerCP;

	/// <summary>
	/// ‰¹º‚ğÄ¶‚·‚é‚â‚Â
	/// </summary>
	class AudioPlayer : public IComponent<AudioPlayerCP, AudioPlayer>
	{
		friend AudioPlayerCP;

	public:
		using IComponent<mtgb::AudioPlayerCP, AudioPlayer>::IComponent;
		//AudioPlayer();
		~AudioPlayer();

		inline AudioPlayer& operator=(const AudioPlayer& _other)
		{
			if (&_other == this)
			{
				return *this;
			}
			this->pSourceVoice_ = _other.pSourceVoice_;
			//			massert(false && "AudioPlayer‚ÌƒRƒs[”­¶");
			return *this;
		}

		/// <summary>
		/// AudioPlayerCP‚©‚çŒÄ‚Î‚ê‚é‰Šú‰»ˆ—
		/// </summary>
		void Initialize() override;

		void SetAudio(const AudioHandle _hAudio);
		void Play();

	private:
		IXAudio2SourceVoice* pSourceVoice_{ nullptr };
		AudioClip* pAudioClip_{ nullptr };
	};

}
