#pragma once
#include "ISystem.h"
#include "ComponentPool.h"
#include "AudioPlayer.h"

namespace mtgb
{
	class AudioPlayer;

	class AudioPlayerCP :
		public ISystem,
		public ComponentPool<AudioPlayer>
	{
	public:
		AudioPlayerCP();
		AudioPlayerCP(const AudioPlayerCP&) = default;
		~AudioPlayerCP();

		void Initialize() override;
		void Update() override;
	};
}
