#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <string>
#include <map>
#include <functional>


struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;

namespace mtgb
{
	class AudioClip;
	struct WaveData;
	struct AudioOneShot;

	using AudioHandle = int32_t;

	class Audio : public ISystem
	{
	public:
		Audio();
		~Audio();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// ロード済みのAudioClipを取得
		/// </summary>
		/// <param name="_hAudio">音声ハンドル</param>
		/// <returns>AudioSourceのポインタ</returns>
		AudioClip* GetAudioClip(const AudioHandle _hAudio);

		/// <summary>
		/// SourceVoiceを作成する
		/// </summary>
		/// <param name="ppSourceVoice">格納用SourceVoiceのポインタ渡し</param>
		/// <param name="_pWaveData">フォーマット用のWaveData</param>
		void CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, const WaveData* _pWaveData);

	private:
		IXAudio2* pXAudio2_;                       // XAudio2のインタフェース
		IXAudio2MasteringVoice* pMasteringVoice_;  // 主ボイス

	public:
		/// <summary>
		/// 音声ファイルをロードする
		/// </summary>
		/// <param name="_fileName">ファイル名</param>
		/// <returns>音声ハンドル</returns>
		static AudioHandle Load(const std::string& _fileName);
		/// <summary>
		/// Waveファイルのバッファを直再生する
		/// </summary>
		/// <param name="pBuffer">Waveファイルのバッファポインタ</param>
		/// <param name="_bufferSize">バッファサイズ</param>
		static void PlayOneShotBuffer(byte* pBuffer, const size_t _bufferSize);
		/// <summary>
		/// 音声ファイルのバッファを直再生する
		/// </summary>
		/// <param name="_fileName">ファイル名</param>
		static void PlayOneShotFile(const std::string& _fileName);

	private:
		/// <summary>
		/// 指定ハンドルの音声をロードする
		/// </summary>
		/// <param name="_hAudio">音声ハンドル</param>
		/// <param name="pBuffer">ロードするバッファ</param>
		/// <param name="_bufferSize">ロードするバッファサイズ</param>
		static void LoadAudioSource(const AudioHandle _hAudio, byte* pBuffer, const size_t _bufferSize);
		/// <summary>
		/// <para>単発音声キューに追加</para>
		/// <para>CPUのSleepプロセスを参考</para>
		/// </summary>
		/// <param name="_oneShot">追加する単発音声</param>
		static void EnqueueOneShot(AudioOneShot* _pOneShot);

	private:
		static AudioHandle handleCounter_;                      // ハンドラ加算用
		static std::map<AudioHandle, AudioClip*> pAudioClips_;  // ハンドルとAudioClipの対応付map
		static std::list<AudioOneShot*> pOneShotQueue_;         // 単発音声のキュー
	};
}
