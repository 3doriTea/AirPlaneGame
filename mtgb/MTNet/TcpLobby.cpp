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
		Log::Write("���r�[���J�Ɏ��s");
		return;
	}

	isRunningLobby_ = true;  // �N���t���O�����g

	lobbyThread_ = std::thread  // �񓯊�����
	{
		[&, this]()
		{
			while (true)
			{
				TcpClient* client{ tcpListener_.AcceptTcpClient() };
				if (client == nullptr)
				{
					Log::Write("��t�I��");
					break;
				}

				// �񓯊��ŃN���C�A���g�Ƃ̃Z�b�V�������J�n
				this->clientSessionThreads_.push_back(std::thread
					{
						[&, this, client]()
						{
							StartSession(client, ++this->playerIdCount_);
						}
					});

				if (this->playerIdCount_ == 0xffui8)
				{
					Log::Write("�Z�b�V�������ʎq���ő�l�ɒB�������ߎ�t�I��");
					break;
				}

				Log::Write("�N���C�A���g�󂯓��ꂽ");
			}

			this->Close();
		}
	};
}

void mtnet::TcpLobby::Close()
{
	tcpListener_.Close();  // �N���C�A���g�󂯓�������

	isRunningLobby_ = false;  // �ғ���~�t���O�����g
	size_t siz = clientSessionThreads_.size();
	for (auto&& clientSession : clientSessionThreads_)
	{
		clientSession.join();  // �񓯊������̏I����҂�
	}

	for (auto&& sendQueue : clientSendQueue_)
	{
		{  // �r������
			std::lock_guard<std::mutex> lock(*mutexClientWriteSendQueue_[sendQueue.first]);
			while (sendQueue.second->size() > 0)
			{
				delete[](*sendQueue.second->back()).pBuffer;  // ���I�m�ۂ��������M�̃o�b�t�@���
				delete sendQueue.second->back();
				sendQueue.second->pop();  // Dequeue
			}
		}
	}
}

void mtnet::TcpLobby::SendToBroadcast(const WriteSendBufferCallback& _callback)
{
	using mtnet::Byte;

	// ������e�������܂��o�b�t�@
	Byte buffer[CLIENT_BUFFER_SIZE]{};

	MemoryStream ms{ buffer, CLIENT_BUFFER_SIZE};
	_callback(static_cast<BinaryWriter*>(&ms));

	for (auto&& clientSendQueue : clientSendQueue_)
	{
		// MEMO: ���M���������Ă�����
		Byte* copyedBuffer{ new Byte[ms.Current()] };
		std::memcpy(copyedBuffer, buffer, ms.Current());  // �Z�b�V�����ʂɓ��I�m�ۂ��ăR�s�[
		// ���MQueue�ɒǉ�(���Ƃ͑҂���)
		ClientSendQueueAdd(clientSendQueue.first, copyedBuffer, ms.Current());

		// TODO: �m�ۂ����������̎Q�Ɖ񐔂łP��̊m�ۂōςދC������A�ǂ���RO����
	}
}

void mtnet::TcpLobby::SendToSession(PlayerId _sessionId, const WriteSendBufferCallback& _callback)
{
	using mtnet::Byte;

	// MEMO: ���M���������Ă�����
	Byte* writeBuffer{ new Byte[CLIENT_BUFFER_SIZE] };  // ������e�o�b�t�@

	MemoryStream ms{ writeBuffer, CLIENT_BUFFER_SIZE };
	_callback(static_cast<BinaryWriter*>(&ms));  // ���M���e�������Ă��炤

	// ���MQueue�ɒǉ�(���Ƃ͑҂���)
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

	// �N���C�A���g���M����������
	clientSendQueue_.insert({ _sessionId, new std::queue<SendQueueElement*>{} });
	// �N���C�A���g�������ݔr������pmutex�̏�����
	mutexClientWriteSendQueue_.insert({ _sessionId, new std::mutex{} });

	Byte* receiveBuffer = new Byte[CLIENT_BUFFER_SIZE];  // ��M�p�̃o�b�t�@
	Byte* sendBuffer = new Byte[CLIENT_BUFFER_SIZE];  // ���M�p�̃o�b�t�@
	mtnet::ReceivedLength receivedLength{};  // �󂯎�����o�C�g��

	MemoryStream sendBufferMS{ sendBuffer, CLIENT_BUFFER_SIZE };
	MemoryStream receiveBufferMS{ receiveBuffer, CLIENT_BUFFER_SIZE };

	// ������𑗐M����
	auto SendString = [&, this](const char* string) -> void
		{
			ZeroMemory(sendBuffer, CLIENT_BUFFER_SIZE);
			std::memcpy(sendBuffer, string, std::strlen(string));
			_client->Send(sendBuffer, CLIENT_BUFFER_SIZE);
		};

	// ��M����������̐擪������r���� (��v:true / false)
	auto ReceiveComparePrefix = [&, this](const char* string) -> bool
		{
			ZeroMemory(receiveBuffer, CLIENT_BUFFER_SIZE);
			receivedLength = _client->Receive(receiveBuffer, CLIENT_BUFFER_SIZE);

			if (std::strlen(string) > std::strlen(receiveBuffer))
			{
				return false;  // �������s��v
			}

			// ��������r (��v:true / false)
			for (int i = 0; i < std::strlen(string); i++)
			{
				if (string[i] != receiveBuffer[i])
				{
					return false;
				}
			}
			return true;
		};

	// TODO: �����̈Í��C�ӂɂ���
	if (ReceiveComparePrefix("Hello.") == false)
	{
		Log::Write("�N���C�A���g�n���h�V�F�C�N?���s");
		return;
	}

	{
		Byte idMessage[]{ 'u', 'r', 'i', 'd', ':', static_cast<Byte>(_sessionId) };
		SendString(idMessage);
	}

	/*
	* �T�[�o�[���̃��[�v
	* Step1. �󂯎��
	* Step2. ����
	* Step3. >>Step1
	*/

	while (isRunningLobby_)
	{
		ZeroMemory(receiveBuffer, CLIENT_BUFFER_SIZE);
		receivedLength = _client->Receive(receiveBuffer, CLIENT_BUFFER_SIZE);

		if (receivedLength <= 0)
		{
			Log::Write("�N���C�A���g���ؒf�������ߏI����...");
			break;
		}

		if (std::string{ receiveBuffer } == std::string{ EMPTY_MESSAGE })
		{
			// �󃁃b�Z�[�W�Ȃ疳��
		}
		else  // �󃁃b�Z�[�W�ł͂Ȃ�
		{
			//Log::Write(receiveBuffer, CLIENT_BUFFER_SIZE);

			receiveBufferMS.Seek(SeekDir::Head);  // ������ǂݎ���悤�ɂ��Ă�����
			onMessageCallback_(
				_sessionId,
				static_cast<BinaryReader*>(&receiveBufferMS));
		}

		if (clientSendQueue_[_sessionId]->size() <= 0)  // ���M������̂��Ȃ�
		{
			_client->Send(
				EMPTY_MESSAGE,
				static_cast<int>(std::strlen(EMPTY_MESSAGE) + 1));  // �ۓ��𑗐M
		}
		else  // ���M������̂�����
		{
			{  // �r������
				std::lock_guard<std::mutex> lock(*mutexClientWriteSendQueue_[_sessionId]);

				// ���M����v�f
				SendQueueElement& sendElement{ *(clientSendQueue_[_sessionId]->back()) };
				_client->Send(
					sendElement.pBuffer,
					static_cast<int>(sendElement.bufferSize));  // ���M

				delete[] sendElement.pBuffer;  // ���I�m�ۂ����o�b�t�@���
				clientSendQueue_[_sessionId]->pop();  // Dequeue
			}
		}
	}

	{  // �r������
		std::lock_guard<std::mutex> lock(*mutexClientWriteSendQueue_[_sessionId]);
		while (clientSendQueue_[_sessionId]->size() > 0)
		{
			delete[](*(clientSendQueue_[_sessionId]->back())).pBuffer;  // ���I�m�ۂ��������M�̃o�b�t�@���
			delete clientSendQueue_[_sessionId]->back();
			clientSendQueue_[_sessionId]->pop();  // Dequeue
		}
		delete clientSendQueue_[_sessionId];
		clientSendQueue_.erase(_sessionId);
	}
	delete mutexClientWriteSendQueue_[_sessionId];
	mutexClientWriteSendQueue_.erase(_sessionId);

	_client->Close();

	delete[] receiveBuffer;  // ��M�o�b�t�@�̉��
	delete[] sendBuffer;  // ���M�o�b�t�@�̉��

	Log::Write("�T�[�o�[���Z�b�V�����I��");
}

void mtnet::TcpLobby::ClientSendQueueAdd(PlayerId _sessionId, mtnet::Byte* _pBuffer, const size_t& _bufferSize)
{
	{  // �r������
		std::lock_guard<std::mutex> lock(*mutexClientWriteSendQueue_[_sessionId]);
		clientSendQueue_[_sessionId]->push(new SendQueueElement{ _pBuffer, _bufferSize });  // Enqueue
	}
}

//const size_t mtnet::TcpLobby::CLIENT_BUFFER_SIZE{ 1024 };  // TODO: magic number
