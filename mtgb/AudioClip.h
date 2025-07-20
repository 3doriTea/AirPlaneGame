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
	/// <para>�����N���b�v (=�����f�[�^)</para>
	/// <para>TODO: Wave�ȊO���ǂݍ��ޏꍇ�A������g��</para>
	/// </summary>
	class AudioClip
	{
		friend AudioPlayer;
		friend Audio;

	public:
		AudioClip();
		~AudioClip();

		/// <summary>
		/// �����f�[�^���N���b�v�Ƀ��[�h����
		/// </summary>
		/// <param name="_ms">�����f�[�^��ms</param>
		void Load(mtbin::MemoryStream& _ms);

		/// <summary>
		/// �����f�[�^�̑��Đ�����(�b)���擾
		/// </summary>
		/// <returns></returns>
		float GetTotalTimeSec() const;
	
	private:
		WaveData* pWaveData_;  // Wave�t�@�C���̃f�[�^
	};
}
