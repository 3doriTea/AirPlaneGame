#include "Audio.h"
#include <xaudio2.h>
#include <fileapi.h>
#include "MTAssert.h"
#include "AudioClip.h"
#include "MTBin/MemoryStream.h"
#include "ReleaseUtility.h"
#include "WaveData.h"
#include "GameTime.h"
#include "AudioOneShot.h"
#include "Debug.h"


mtgb::Audio::Audio() :
	pXAudio2_{ nullptr },
	pMasteringVoice_{ nullptr }
{
}

mtgb::Audio::~Audio()
{
	if (pMasteringVoice_ != nullptr)
	{
		pMasteringVoice_->DestroyVoice();
		pMasteringVoice_ = nullptr;
	}
	
}

void mtgb::Audio::Initialize()
{
	HRESULT hResult{};

	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	massert(SUCCEEDED(hResult)  // COMの初期化に成功
		&& "COMの初期化に失敗 @Audio::Initialize");

	hResult = XAudio2Create(&pXAudio2_);
	massert(SUCCEEDED(hResult)  // XAudio2の作成に成功
		&& "XAudio2の作成に失敗 @Audio::Initialize");

	hResult = pXAudio2_->CreateMasteringVoice(&pMasteringVoice_);
	massert(SUCCEEDED(hResult)  // MasteringVoiceの作成に成功
		&& "MasteringVoiceの作成に失敗 @Audio::Initialize");
}

void mtgb::Audio::Update()
{
	if (pOneShotQueue_.size() <= 0)
	{
		return;  // キューが空なら回帰
	}

	//  REF: CPUのSleepプロセス

	// 再生終了をデキューする
	auto itr = pOneShotQueue_.begin();
	while (true)
	{
		(*itr)->timeLeft -= Time::DeltaTimeF();

		float diff{ (*itr)->timeLeft };  // 差分
		if (diff > 0)  // 差分が0より大きい = 再生中
		{
			break;  // 再生終了していないため離脱
		}
		else  // 差分が0以下 = 再生終了
		{
			delete (*itr);  // 解放する
			itr = pOneShotQueue_.erase(pOneShotQueue_.begin());  // 消す
			if (itr == pOneShotQueue_.end())
			{
				break;  // 後続がない = キューが空なら離脱
			}
			(*itr)->timeLeft += diff;  // 差分を後続に反映
			continue;  // 後続も終了している可能性があるため継続
		}
	}
}

mtgb::AudioClip* mtgb::Audio::GetAudioClip(const AudioHandle _hAudio)
{
	massert((pAudioClips_.count(_hAudio) > 0)
		&& "指定されたハンドルは無効/まだロードされていない");

	return pAudioClips_[_hAudio];
}

void mtgb::Audio::CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, const WaveData* _pWaveData)
{
	HRESULT hResult{};

	hResult = pXAudio2_->CreateSourceVoice(ppSourceVoice, &_pWaveData->waveFormat);
	massert(SUCCEEDED(hResult)  // SourceVoice作成に成功
		&& "SourceVoice作成に失敗 @Audio::SetSourceVoice");
}

mtgb::AudioHandle mtgb::Audio::Load(const std::string& _fileName)
{
	handleCounter_++;
	pAudioClips_.insert({ handleCounter_, new AudioClip{} });

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-createfilea
	HANDLE hFile = CreateFile(
		_fileName.c_str(),      // ファイル名
		GENERIC_READ,           // 読み取りますよー
		FILE_SHARE_READ,        // Closeされるまで、他のアプリはファイルの読み取りだけしていいよー
		nullptr,                // セキュリティ属性用の構造体ポインタを指定
		OPEN_EXISTING,          // 開く - ファイルが無かったら失敗
		FILE_ATTRIBUTE_NORMAL,  // 普通のファイル属性
		NULL);                  // 既存のファイルを開く場合は関係ないやつ

	if (hFile == INVALID_HANDLE_VALUE)
	{
		massert(false && "ファイルOpenに失敗 @Audio::Load");
		return INVALID_HANDLE;
	}

	BOOL succeed{ FALSE };

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
	LARGE_INTEGER fileSize{};  // ファイルサイズ格納用
	succeed = GetFileSizeEx(hFile, &fileSize);
	if (succeed == FALSE)
	{
		massert(false && "ファイルサイズ取得に失敗 @Audio::Load");
		return INVALID_HANDLE;
	}

	DWORD readedSize{ 0 };  // 実際に読み取れたバイト数

	byte* pBuffer{ new byte[fileSize.QuadPart] };  // バッファ動的確保

	succeed = ReadFile(hFile, pBuffer, static_cast<DWORD>(fileSize.QuadPart), &readedSize, NULL);
	if (succeed == FALSE || readedSize != fileSize.QuadPart)
	{
		massert(false && "ファイルの読み取りに失敗 @Audio::Load");
		delete[] pBuffer;  // バッファ解放
		return INVALID_HANDLE;
	}

	CloseHandle(hFile);  // ファイルを閉じる

	// いざ読み込み
	LoadAudioSource(handleCounter_, pBuffer, fileSize.QuadPart);

	delete[] pBuffer;  // バッファ解放

	return handleCounter_;
}

void mtgb::Audio::PlayOneShotBuffer(byte* pBuffer, const size_t _bufferSize)
{
	AudioOneShot* oneShot{ new AudioOneShot{} };

	mtbin::MemoryStream ms{ pBuffer, _bufferSize };
	oneShot->pAudioClip->Load(ms);

	Game::System<Audio>().CreateSourceVoice(
		&oneShot->pSourceVoice,
		oneShot->pAudioClip->pWaveData_);

	oneShot->timeLeft = static_cast<float>(oneShot->pAudioClip->GetTotalTimeSec());

	EnqueueOneShot(oneShot);  // 自動解放キューに追加

	const XAUDIO2_BUFFER BUFFER
	{
		.Flags = XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(oneShot->pAudioClip->pWaveData_->bufferSize),
		.pAudioData = oneShot->pAudioClip->pWaveData_->pBuffer,
		.LoopCount = 0,
	};

	HRESULT hResult{};
	hResult = oneShot->pSourceVoice->SubmitSourceBuffer(&BUFFER);

	massert(SUCCEEDED(hResult)  // SourceBufferのセットに成功
		&& "SourceBufferのセットに失敗 @Audio::PlayOneShotBuffer");

	oneShot->pSourceVoice->Start();  // 再生
}

void mtgb::Audio::PlayOneShotFile(const std::string& _fileName)
{
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-createfilea
	HANDLE hFile = CreateFile(
		_fileName.c_str(),      // ファイル名
		GENERIC_READ,           // 読み取りますよー
		FILE_SHARE_READ,        // Closeされるまで、他のアプリはファイルの読み取りだけしていいよー
		nullptr,                // セキュリティ属性用の構造体ポインタを指定
		OPEN_EXISTING,          // 開く - ファイルが無かったら失敗
		FILE_ATTRIBUTE_NORMAL,  // 普通のファイル属性
		NULL);                  // 既存のファイルを開く場合は関係ないやつ

	if (hFile == INVALID_HANDLE_VALUE)
	{
		massert(false && "ファイルOpenに失敗 @Audio::Load");
		return;
	}

	BOOL succeed{ FALSE };

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
	LARGE_INTEGER fileSize{};  // ファイルサイズ格納用
	succeed = GetFileSizeEx(hFile, &fileSize);
	if (succeed == FALSE)
	{
		massert(false && "ファイルサイズ取得に失敗 @Audio::Load");
		return;
	}

	DWORD readedSize{ 0 };  // 実際に読み取れたバイト数

	byte* pBuffer{ new byte[fileSize.QuadPart] };  // バッファ動的確保

	succeed = ReadFile(hFile, pBuffer, static_cast<DWORD>(fileSize.QuadPart), &readedSize, NULL);
	if (succeed == FALSE || readedSize != fileSize.QuadPart)
	{
		massert(false && "ファイルの読み取りに失敗 @Audio::Load");
		delete[] pBuffer;  // バッファ解放
		return;
	}

	CloseHandle(hFile);  // ファイルを閉じる

	// いざ再生
	PlayOneShotBuffer(pBuffer, fileSize.QuadPart);

	delete[] pBuffer;  // バッファ解放
}

void mtgb::Audio::LoadAudioSource(const AudioHandle _hAudio, byte* pBuffer, const size_t _bufferSize)
{
	mtbin::MemoryStream ms{ pBuffer, _bufferSize };
	pAudioClips_[_hAudio]->Load(ms);
}

void mtgb::Audio::EnqueueOneShot(AudioOneShot* _pOneShot)
{
	float lefter{ _pOneShot->timeLeft };  // 減算用
	float righter{ 0 };                   // 加算用
	// 適切な挿入ポイントを見つける
	for (auto itr = pOneShotQueue_.begin(); itr != pOneShotQueue_.end(); itr++)
	{
		if (lefter <= righter + (*itr)->timeLeft)
		{
			_pOneShot->timeLeft = lefter - righter;
			itr = pOneShotQueue_.insert(itr, _pOneShot);
			itr++;
			(*itr)->timeLeft -= _pOneShot->timeLeft;
			return;
		}
		righter += (*itr)->timeLeft;
	}
	// 見つからなかったら末端に追加
	pOneShotQueue_.push_back(_pOneShot);
}

mtgb::AudioHandle mtgb::Audio::handleCounter_{ mtgb::INVALID_HANDLE };
std::map<mtgb::AudioHandle, mtgb::AudioClip*> mtgb::Audio::pAudioClips_{};
std::list<mtgb::AudioOneShot*> mtgb::Audio::pOneShotQueue_{};
