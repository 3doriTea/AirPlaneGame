#include "AudioPlayer.h"
#include <xaudio2.h>
#include "Audio.h"
#include "AudioClip.h"
#include "WaveData.h"
#include "ReleaseUtility.h"


mtgb::AudioPlayer::~AudioPlayer()
{
	if (pSourceVoice_ != nullptr)
	{
		pSourceVoice_->DestroyVoice();
		pSourceVoice_ = nullptr;
	}

	SAFE_DELETE(pAudioClip_);
}

void mtgb::AudioPlayer::Initialize()
{
}

void mtgb::AudioPlayer::SetAudio(const AudioHandle _hAudio)
{
	Audio& audio{ Game::System<Audio>() };

	pAudioClip_ = audio.GetAudioClip(_hAudio);

	audio.CreateSourceVoice(&pSourceVoice_, pAudioClip_->pWaveData_);

	const XAUDIO2_BUFFER BUFFER
	{
		.Flags = XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(pAudioClip_->pWaveData_->bufferSize),
		.pAudioData = pAudioClip_->pWaveData_->pBuffer,
		.LoopCount = 0,
	};

	HRESULT hResult{};
	hResult = pSourceVoice_->SubmitSourceBuffer(&BUFFER);

	massert(SUCCEEDED(hResult)  // SourceBufferのセットに成功
		&& "SourceBufferのセットに失敗 @AudioPlayer::SetAudio");
}

void mtgb::AudioPlayer::Play()
{
	pSourceVoice_->Start();
}
