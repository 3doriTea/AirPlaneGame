#include "AudioClip.h"
#include "ReleaseUtility.h"
#include "cmtgb.h"
#include "WaveData.h"


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
	using mtbin::Utility::CompareId;

	// 既に読み込まれているデータがあるなら解放
	SAFE_DELETE(pWaveData_);
	pWaveData_ = new WaveData{};

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
