#include "AudioClip.h"
#include "ReleaseUtility.h"
#include "cmtgb.h"
#include "WaveData.h"

#define DR_MP3_IMPLEMENTATION
#include "../dr_mp3.h"

using mtbin::Utility::CompareId;

mtgb::AudioClip::AudioClip() :
	pWaveData_{ nullptr }
{
}

mtgb::AudioClip::~AudioClip()
{
	SAFE_DELETE(pWaveData_);
}

void mtgb::AudioClip::Load(mtbin::MemoryStream& _ms)
{
	SAFE_DELETE(pWaveData_);
	pWaveData_ = new WaveData{};
	_ms.Seek(0);

	// 先頭4バイトを読み取る
	byte header[4];
	_ms.Read(header, sizeof(header), 4);

	// フォーマット判定
	if (CompareId<4>(header, "RIFF"))
	{
		// WAVよみこみ 

		_ms.Seek(0);
		LoadWave(_ms, header);
	}
	else if (CompareId<3>(header, "ID3") || (header[0] == 0xFF && (header[1] % 0xE0) == 0xE0))
	{
		_ms.Seek(0);
		LoadMp3(_ms, header);
	}
	else
	{
		massert(false && "対応していない音声フォーマットです @AudioClip::Load");
	}
}

void mtgb::AudioClip::LoadWave(mtbin::MemoryStream& _ms, const byte* _first4)
{
	// チャンク識別子は 4 byte
	static const size_t ID_SIZE{ 4 };
	
	// チャンクヘッダ情報
	struct ChunkHeader
	{
		ChunkHeader() :
			id{},
			size{ 0 }
		{}

		byte id[ID_SIZE];  // チャンク識別子
		uint32_t size;  // チャンクボディのサイズ
	};

	// RIFF チャンク
	ChunkHeader riff{};
	_ms.Read(riff.id, sizeof(riff.id), ID_SIZE);
	massert(CompareId<ID_SIZE>(riff.id, "RIFF")
		&& "RIFF チャンクIdの不一致 @AudioClip::Load");
	riff.size = _ms.Read<uint32_t>();

	// WAVE チャンク
	byte wave[ID_SIZE]{};
	_ms.Read(wave, sizeof(wave), ID_SIZE);
	massert(CompareId<ID_SIZE>(wave, "WAVE")
		&& "WAVE チャンクIdの不一致 @AudioClip::Load");

	// フォーマットチャンク
	ChunkHeader format{};
	_ms.Read(format.id, sizeof(format.id), ID_SIZE);
	massert(CompareId<ID_SIZE>(format.id, "fmt ")
		&& "フォーマット チャンクIdの不一致 @AudioClip::Load");
	format.size = _ms.Read<uint32_t>();
	massert((format.size == 0x10)
		&& "対応していないフォーマットサイズ @AudioClip::Load");
	_ms.Read(reinterpret_cast<byte*>(&pWaveData_->waveFormat), sizeof(WAVEFORMATEX), format.size);

	// wBitsPerSampleを設定
	/*pWaveData_->waveFormat.wBitsPerSample =
		pWaveData_->waveFormat.nBlockAlign * 8 / pWaveData_->waveFormat.nChannels;*/
	pWaveData_->waveFormat.nBlockAlign =
		pWaveData_->waveFormat.nChannels * pWaveData_->waveFormat.wBitsPerSample / 8;
	pWaveData_->waveFormat.nAvgBytesPerSec = pWaveData_->waveFormat.nSamplesPerSec * pWaveData_->waveFormat.nBlockAlign;

	// dataチャンクを探す
	ChunkHeader header{};
	while (true)
	{
		_ms.Read(header.id, sizeof(header.id), ID_SIZE);
		header.size = _ms.Read<uint32_t>();

		if (CompareId<ID_SIZE>(header.id, "data"))
		{
			break;  // dataチャンクに到達したら離脱
		}
		else
		{
			_ms.Seek(_ms.Current() + header.size);  // dataチャンクでないなら読み飛ばし
		}
	}

	pWaveData_->bufferSize = header.size;
	pWaveData_->pBuffer = new byte[header.size]{};
	_ms.Read(pWaveData_->pBuffer, header.size, header.size);
}

void mtgb::AudioClip::LoadMp3(mtbin::MemoryStream& _ms, const byte* _first4)
{
	// MemoryStreamからmp3データを読み込む
	size_t size = _ms.GetLength();
	byte* mp3_data = new byte[size];
	_ms.Read(mp3_data, size, size);

	// drmp3のデコード初期化
	drmp3 config;
	if(!drmp3_init_memory(&config, mp3_data, size, NULL))
	{
		massert(false && "MP3の初期化に失敗 @AudioClip::LoadMp3");
		delete[] mp3_data;
	}

	// 総PCMサンプル数を取得
	drmp3_uint64 total_samples = drmp3_get_pcm_frame_count(&config);
	// バッファサイズを計算
	size_t pcm_sample_count = (size_t)(total_samples * config.channels);
	size_t pcm_bytes = pcm_sample_count * sizeof(short);

	// PCM出力バッファ確保
	pWaveData_->bufferSize = pcm_bytes;
	pWaveData_->pBuffer = new byte[pcm_bytes];

	// PCMデータを読み込む
	drmp3_read_pcm_frames_s16(&config, total_samples, reinterpret_cast<short*>(pWaveData_->pBuffer));

	// WAVEフォーマット情報設定
	WAVEFORMATEX& wf = pWaveData_->waveFormat;
	wf.wFormatTag = 1;
	wf.nChannels = static_cast<WORD>(config.channels);
	wf.nSamplesPerSec = static_cast<DWORD>(config.sampleRate);
	wf.wBitsPerSample = 16;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

	// 後始末
	drmp3_uninit(&config);
	delete[] mp3_data; // メモリリーク防止のために開放
}

float mtgb::AudioClip::GetTotalTimeSec() const
{
	if (pWaveData_ == nullptr)
	{
		return 0;
	}
	massert(pWaveData_->waveFormat.nAvgBytesPerSec != 0 && "0除算してしまいます。");
	// データサイズ / 1秒間あたりの読みバイト数 = 総再生時間
	return pWaveData_->bufferSize / static_cast<float>(pWaveData_->waveFormat.nAvgBytesPerSec);
}
