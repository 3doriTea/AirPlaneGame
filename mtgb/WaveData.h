#pragma once
#include <xaudio2.h>

namespace mtgb
{
	/// <summary>
	/// Waveファイルのデータ格納構造体
	/// </summary>
	struct WaveData
	{
		WaveData();
		~WaveData();

		WAVEFORMATEX waveFormat;  // フォーマット情報
		byte* pBuffer;            // データバッファポインタ配列
		size_t bufferSize;        // データバッファサイズ
	};
}
