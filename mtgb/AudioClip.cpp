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

	// ���ɓǂݍ��܂�Ă���f�[�^������Ȃ���
	SAFE_DELETE(pWaveData_);
	pWaveData_ = new WaveData{};

	// �`�����N���ʎq�� 4 byte
	static const size_t ID_SIZE{ 4 };
	
	// �`�����N�w�b�_���
	struct ChunkHeader
	{
		ChunkHeader() :
			id{},
			size{ 0 }
		{}

		byte id[ID_SIZE];  // �`�����N���ʎq
		uint32_t size;  // �`�����N�{�f�B�̃T�C�Y
	};

	// RIFF �`�����N
	ChunkHeader riff{};
	_ms.Read(riff.id, sizeof(riff.id), ID_SIZE);
	massert(CompareId<ID_SIZE>(riff.id, "RIFF")
		&& "RIFF �`�����NId�̕s��v @AudioClip::Load");
	riff.size = _ms.Read<uint32_t>();

	// WAVE �`�����N
	byte wave[ID_SIZE]{};
	_ms.Read(wave, sizeof(wave), ID_SIZE);
	massert(CompareId<ID_SIZE>(wave, "WAVE")
		&& "WAVE �`�����NId�̕s��v @AudioClip::Load");

	// �t�H�[�}�b�g�`�����N
	ChunkHeader format{};
	_ms.Read(format.id, sizeof(format.id), ID_SIZE);
	massert(CompareId<ID_SIZE>(format.id, "fmt ")
		&& "�t�H�[�}�b�g �`�����NId�̕s��v @AudioClip::Load");
	format.size = _ms.Read<uint32_t>();
	massert((format.size == 0x10)
		&& "�Ή����Ă��Ȃ��t�H�[�}�b�g�T�C�Y @AudioClip::Load");
	_ms.Read(reinterpret_cast<byte*>(&pWaveData_->waveFormat), sizeof(WAVEFORMATEX), format.size);

	// wBitsPerSample��ݒ�
	/*pWaveData_->waveFormat.wBitsPerSample =
		pWaveData_->waveFormat.nBlockAlign * 8 / pWaveData_->waveFormat.nChannels;*/
	pWaveData_->waveFormat.nBlockAlign =
		pWaveData_->waveFormat.nChannels * pWaveData_->waveFormat.wBitsPerSample / 8;
	pWaveData_->waveFormat.nAvgBytesPerSec = pWaveData_->waveFormat.nSamplesPerSec * pWaveData_->waveFormat.nBlockAlign;

	// data�`�����N��T��
	ChunkHeader header{};
	while (true)
	{
		_ms.Read(header.id, sizeof(header.id), ID_SIZE);
		header.size = _ms.Read<uint32_t>();

		if (CompareId<ID_SIZE>(header.id, "data"))
		{
			break;  // data�`�����N�ɓ��B�����痣�E
		}
		else
		{
			_ms.Seek(_ms.Current() + header.size);  // data�`�����N�łȂ��Ȃ�ǂݔ�΂�
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
	massert(pWaveData_->waveFormat.nAvgBytesPerSec != 0 && "0���Z���Ă��܂��܂��B");
	// �f�[�^�T�C�Y / 1�b�Ԃ�����̓ǂ݃o�C�g�� = ���Đ�����
	return pWaveData_->bufferSize / static_cast<float>(pWaveData_->waveFormat.nAvgBytesPerSec);
}
