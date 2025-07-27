#include "TcpJoiner.h"
#include "Log.h"
#include <chrono>

namespace
{
	constexpr size_t CLIENT_BUFFER_SIZE{ 1024 };
	constexpr int EMPTY_SLEEP_TIME_MILLSEC{ 100 };
}

mtnet::TcpJoiner::TcpJoiner(
	const IPEndPoint& _local,
	const char* _pCheckCode) :
	client_{ _local },
	CHECK_CODE{ _pCheckCode },
	isRunningJoiner_{ false },
	sessionId_{ 0 }
{
}

mtnet::TcpJoiner::~TcpJoiner()
{
}

void mtnet::TcpJoiner::Connect(const IPEndPoint& _remote)
{
	using mtnet::Log;

	if (isRunningJoiner_ == true)
	{
		throw "���ɎQ����";
	}

	client_.Connect(_remote);  // �Q��

	isRunningJoiner_ = true;  // �Q�����t���O�����g

	sessionThread_ = std::thread
	{
		[&, this]()
		{
			using mtnet::Byte;
			using SeekDir = mtnet::MemoryStream::SeekDir;

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
					client_.Send(sendBuffer, CLIENT_BUFFER_SIZE);
				};

			// ��M����������̐擪������r���� (��v:true / false)
			auto ReceiveComparePrefix = [&, this](const char* string) -> bool
				{
					ZeroMemory(receiveBuffer, CLIENT_BUFFER_SIZE);
					receivedLength = client_.Receive(receiveBuffer, CLIENT_BUFFER_SIZE);

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
			SendString("Hello.");

			if (ReceiveComparePrefix("urid:") == false)
			{
				Log::Write("�T�[�o�[�Ƃ̃n���h�V�F�C�N?���s");
				return;
			}
			sessionId_ = static_cast<PlayerId>(receiveBuffer[5]);

			/*
			* �N���C�A���g���̃��[�v
			* Step1. ����
			* Step2. �󂯎��
			* Step3. >>Step1
			*/

			while (isRunningJoiner_)
			{
				if (sendQueue_.size() <= 0)  // ���M������̂��Ȃ�
				{
					client_.Send(
						EMPTY_MESSAGE,
						static_cast<int>(std::strlen(EMPTY_MESSAGE) + 1));  // �ۓ��𑗐M

					std::this_thread::sleep_for(std::chrono::milliseconds(
						EMPTY_SLEEP_TIME_MILLSEC));  // ������Ƒ҂�
				}
				else  // ���M������̂�����
				{
					{  // �r������
						std::lock_guard<std::mutex> lock(mutexWriteSendQueue_);

						// ���M����v�f
						SendQueueElement& sendElement{ sendQueue_.back() };
						client_.Send(
							sendElement.pBuffer,
							static_cast<int>(sendElement.bufferSize));  // ���M

						delete[] sendElement.pBuffer;  // ���I�m�ۂ����o�b�t�@���
						sendQueue_.pop();  // Dequeue
					}
				}

				ZeroMemory(receiveBuffer, CLIENT_BUFFER_SIZE);
				receiveBufferMS.Seek(SeekDir::Head);
				receivedLength = client_.Receive(receiveBuffer, CLIENT_BUFFER_SIZE);

				if (receivedLength <= 0)
				{
					Log::Write("�T�[�o�[���ؒf�������ߏI����...");
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
					onRecievedCallback_(static_cast<BinaryReader*>(&receiveBufferMS));
				}
			}

			client_.Close();

			delete[] receiveBuffer;  // ��M�o�b�t�@�̉��
			delete[] sendBuffer;  // ���M�o�b�t�@�̉��

			Log::Write("�N���C�A���g���Z�b�V�����I��");
		}
	};
}

void mtnet::TcpJoiner::Close()
{
	if (isRunningJoiner_ == true)
	{
		isRunningJoiner_ = false;  // �ޏo�t���O�����g
		sessionThread_.join();  // �񓯊�������҂�
	}
}

void mtnet::TcpJoiner::Send(const WriteSendBufferCallback& _callback)
{
	using mtnet::Byte;

	// MEMO: ���M���������Ă�����
	Byte* writeBuffer{ new Byte[CLIENT_BUFFER_SIZE]{} };  // ������e�o�b�t�@

	MemoryStream ms{ writeBuffer, CLIENT_BUFFER_SIZE };
	_callback(static_cast<BinaryWriter*>(&ms));  // ���M���e�������Ă��炤 

	//Log::Write(writeBuffer, CLIENT_BUFFER_SIZE);

	{  // �r������
		std::lock_guard<std::mutex> lock(mutexWriteSendQueue_);
		sendQueue_.push({ writeBuffer, CLIENT_BUFFER_SIZE });
	}
}

void mtnet::TcpJoiner::OnRecieve(const OnRecievedCallback& _callback)
{
	onRecievedCallback_ = _callback;
}

//const size_t mtnet::TcpJoiner::CLIENT_BUFFER_SIZE{ 1024 };
