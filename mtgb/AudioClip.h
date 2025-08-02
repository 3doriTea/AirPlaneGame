#pragma once
#include <xaudio2.h>
#include <string>
#include "MTBin/MemoryStream.h"

typedef struct tWAVEFORMATEX WAVEFORMATEX;

namespace mtgb
{
	struct WaveData;
	class AudioPlayer;
	class Audio;

	/// <summary>
	/// <para>音声クリップ (=音声データ)</para>
	/// <para>TODO: Wave以外も読み込む場合、これを拡張</para>
	/// </summary>
	class AudioClip
	{
		friend AudioPlayer;
		friend Audio;

	public:
		AudioClip();
		~AudioClip();

		/// <summary>
		/// 音声データをクリップにロードする
		/// </summary>
		/// <param name="_ms">音声データのms</param>
		void Load(mtbin::MemoryStream& _ms);

		/// <summary>
		/// WAV読み込み
		/// </summary>
		/// <param name="_ms">WAVのメモリストリーム</param>
		/// <param name="_first4"> 最初の4バイトの識別子</param>
		void LoadWave(mtbin::MemoryStream& _ms, const byte* _first4);

		/// <summary>
		/// mp3読み込み
		/// </summary>
		/// <param name="_ms">mp3のメモリストリーム</param>
		/// <param name="_first4">最初の4バイトの識別子</param>
		void LoadMp3(mtbin::MemoryStream& _ms, const byte* _first4);

		/// <summary>
		/// 音声データの総再生時間(秒)を取得
		/// </summary>
		/// <returns></returns>
		float GetTotalTimeSec() const;
	
	private:
		WaveData* pWaveData_;  // Waveファイルのデータ
	};
}
