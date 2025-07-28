#include "TcpLobby.h"
#include "Utility.h"
#include "Log.h"
#include "TcpClient.h"
#include "Common.h"
#include "MTBin/MemoryStream.h"
#include "BinaryWriter.h"

namespace
{
	constexpr size_t CLIENT_BUFFER_SIZE{ 1024 };
}

mtnet::TcpLobby::TcpLobby(
	const IPEndPoint& _local,
	const char* _pCheckCode) :
	tcpListener_{ _local },
	onJoinedPlayerCallback_{},
	onMessageCallback_{},
	clientSendQueue_{},
	mutexClientWriteSendQueue_{},
	lobbyThread_{},
	clientSessionThreads_{},
	isRunningLobby_{ false },
	CHECK_CODE{ _pCheckCode },
	playerIdCount_{ 0 }
{
}

mtnet::TcpLobby::~TcpLobby()
{
}

std::string mtnet::TcpLobby::GetLobbyCode() const
{
	return mtnet::GetLobbyCode(tcpListener_);
}

void mtnet::TcpLobby::Open()
{
	using mtnet::Log;

	if (tcpListener_.TryStartListen() == false)
	{
		Log::Write("ロビー公開に失敗");
		return;
	}

	isRunningLobby_ = true;  // 起動フラグメント

	lobbyThread_ = std::thread  // 非同期処理
	{
		[&, this]()
		{
			while (true)
			{
				TcpClient* client{ tcpListener_.AcceptTcpClient() };
				if (client == nullptr)
				{
					Log::Write("受付終了");
					break;
				}

				// 非同期でクライアントとのセッションを開始
				this->clientSessionThreads_.push_back(std::thread
					{
						[&, this, client]()
						{
							StartSession(client, ++this->playerIdCount_);
						}
					});

				if (this->playerIdCount_ == 0xffui8)
				{
					Log::Write("セッション識別子が最大値に達したため受付終了");
					break;
				}

				Log::Write("クライアント受け入れた");
			}

			this->Close();
		}
	};
}

void mtnet::TcpLobby::Close()
{
	tcpListener_.Close();  // クライアント受け入れを閉じる

	isRunningLobby_ = false;  // 稼働停止フラグメント
	size_t siz = clientSessionThreads_.size();
	for (auto&& clientSession : clientSessionThreads_)
	{
		clientSession.join();  // 非同期処理の終了を待つ
	}

	for (auto&& sendQueue : clientSendQueue_)
	{
		{  // 排他制御
			std::lock_guard<std::mutex> lock(*mutexClientWriteSendQueue_[sendQueue.first]);
			while (sendQueue.second->size() > 0)
			{
				delete[](*sendQueue.second->back()).pBuffer;  // 動的確保した未送信のバッファ解放
				delete sendQueue.second->back();
				sendQueue.second->pop();  // Dequeue
			}
		}
	}
}

void mtnet::TcpLobby::SendToBroadcast(const WriteSendBufferCallback& _callback)
{
	using mtnet::Byte;

	// 送る内容書き込ませバッファ
	Byte buffer[CLIENT_BUFFER_SIZE]{};

	MemoryStream ms{ buffer, CLIENT_BUFFER_SIZE};
	_callback(static_cast<BinaryWriter*>(&ms));

	for (auto&& clientSendQueue : clientSendQueue_)
	{
		// MEMO: 送信次第解放してくれるよ
		Byte* copyedBuffer{ new Byte[ms.Current()] };
		std::memcpy(copyedBuffer, buffer, ms.Current());  // セッション別に動的確保してコピー
		// 送信Queueに追加(あとは待つだけ)
		ClientSendQueueAdd(clientSendQueue.first, copyedBuffer, ms.Current());

		// TODO: 確保したメモリの参照回数で１回の確保で済む気がする、どうせROだし
	}
}

void mtnet::TcpLobby::SendToSession(PlayerId _sessionId, const WriteSendBufferCallback& _callback)
{
	using mtnet::Byte;

	// MEMO: 送信次第解放してくれるよ
	Byte* writeBuffer{ new Byte[CLIENT_BUFFER_SIZE] };  // 送る内容バッファ

	MemoryStream ms{ writeBuffer, CLIENT_BUFFER_SIZE };
	_callback(static_cast<BinaryWriter*>(&ms));  // 送信内容を書いてもらう

	// 送信Queueに追加(あとは待つだけ)
	ClientSendQueueAdd(_sessionId, writeBuffer, ms.Current());
}

void mtnet::TcpLobby::OnJoinedPlayer(const OnJoinedPlayerCallback& _callback)
{
	onJoinedPlayerCallback_ = _callback;
}

void mtnet::TcpLobby::OnMessage(const OnMessageCallback& _callback)
{
	onMessageCallback_ = _callback;
}

void mtnet::TcpLobby::StartSession(TcpClient* _client, PlayerId _sessionId)
{
	using mtnet::Byte;
	using SeekDir = mtnet::MemoryStream::SeekDir;

	// クライアント送信情報を初期化
	clientSendQueue_.insert({ _sessionId, new std::queue<SendQueueElement*>{} });
	// クライアント書き込み排他制御用mutexの初期化
	mutexClientWriteSendQueue_.insert({ _sessionId, new std::mutex{} });

	Byte* receiveBuffer = new Byte[CLIENT_BUFFER_SIZE];  // 受信用のバッファ
	Byte* sendBuffer = new Byte[CLIENT_BUFFER_SIZE];  // 送信用のバッファ
	mtnet::ReceivedLength receivedLength{};  // 受け取ったバイト数

	MemoryStream sendBufferMS{ sendBuffer, CLIENT_BUFFER_SIZE };
	MemoryStream receiveBufferMS{ receiveBuffer, CLIENT_BUFFER_SIZE };

	// 文字列を送信する
	auto SendString = [&, this](const char* string) -> void
		{
			ZeroMemory(sendBuffer, CLIENT_BUFFER_SIZE);
			std::memcpy(sendBuffer, string, std::strlen(string));
			_client->Send(sendBuffer, CLIENT_BUFFER_SIZE);
		};

	// 受信した文字列の先頭だけ比較する (一致:true / false)
	auto ReceiveComparePrefix = [&, this](const char* string) -> bool
		{
			ZeroMemory(receiveBuffer, CLIENT_BUFFER_SIZE);
			receivedLength = _client->Receive(receiveBuffer, CLIENT_BUFFER_SIZE);

			if (std::strlen(string) > std::strlen(receiveBuffer))
			{
				return false;  // 文字数不一致
			}

			// 文字列を比較 (一致:true / false)
			for (int i = 0; i < std::strlen(string); i++)
			{
				if (string[i] != receiveBuffer[i])
				{
					return false;
				}
			}
			return true;
		};

	// TODO: ここの暗号任意にする
	if (ReceiveComparePrefix("Hello.") == false)
	{
		Log::Write("クライアントハンドシェイク?失敗");
		return;
	}

	{
		Byte idMessage[]{ 'u', 'r', 'i', 'd', ':', static_cast<Byte>(_sessionId) };
		SendString(idMessage);
	}

	/*
	* サーバー側のループ
	* Step1. 受け取る
	* Step2. 送る
	* Step3. >>Step1
	*/

	while (isRunningLobby_)
	{
		ZeroMemory(receiveBuffer, CLIENT_BUFFER_SIZE);
		receivedLength = _client->Receive(receiveBuffer, CLIENT_BUFFER_SIZE);

		if (receivedLength <= 0)
		{
			Log::Write("クライアントが切断したため終了中...");
			break;
		}

		if (std::string{ receiveBuffer } == std::string{ EMPTY_MESSAGE })
		{
			// 空メッセージなら無視
		}
		else  // 空メッセージではない
		{
			//Log::Write(receiveBuffer, CLIENT_BUFFER_SIZE);

			receiveBufferMS.Seek(SeekDir::Head);  // 頭から読み取れるようにしてあげる
			onMessageCallback_(
				_sessionId,
				static_cast<BinaryReader*>(&receiveBufferMS));
		}

		if (clientSendQueue_[_sessionId]->size() <= 0)  // 送信するものがない
		{
			_client->Send(
				EMPTY_MESSAGE,
				static_cast<int>(std::strlen(EMPTY_MESSAGE) + 1));  // 鼓動を送信
		}
		else  // 送信するものがある
		{
			{  // 排他制御
				std::lock_guard<std::mutex> lock(*mutexClientWriteSendQueue_[_sessionId]);

				// 送信する要素
				SendQueueElement& sendElement{ *(clientSendQueue_[_sessionId]->back()) };
				_client->Send(
					sendElement.pBuffer,
					static_cast<int>(sendElement.bufferSize));  // 送信

				delete[] sendElement.pBuffer;  // 動的確保したバッファ解放
				clientSendQueue_[_sessionId]->pop();  // Dequeue
			}
		}
	}

	{  // 排他制御
		std::lock_guard<std::mutex> lock(*mutexClientWriteSendQueue_[_sessionId]);
		while (clientSendQueue_[_sessionId]->size() > 0)
		{
			delete[](*(clientSendQueue_[_sessionId]->back())).pBuffer;  // 動的確保した未送信のバッファ解放
			delete clientSendQueue_[_sessionId]->back();
			clientSendQueue_[_sessionId]->pop();  // Dequeue
		}
		delete clientSendQueue_[_sessionId];
		clientSendQueue_.erase(_sessionId);
	}
	delete mutexClientWriteSendQueue_[_sessionId];
	mutexClientWriteSendQueue_.erase(_sessionId);

	_client->Close();

	delete[] receiveBuffer;  // 受信バッファの解放
	delete[] sendBuffer;  // 送信バッファの解放

	Log::Write("サーバー側セッション終了");
}

void mtnet::TcpLobby::ClientSendQueueAdd(PlayerId _sessionId, mtnet::Byte* _pBuffer, const size_t& _bufferSize)
{
	{  // 排他制御
		std::lock_guard<std::mutex> lock(*mutexClientWriteSendQueue_[_sessionId]);
		clientSendQueue_[_sessionId]->push(new SendQueueElement{ _pBuffer, _bufferSize });  // Enqueue
	}
}

//const size_t mtnet::TcpLobby::CLIENT_BUFFER_SIZE{ 1024 };  // TODO: magic number
