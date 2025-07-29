#pragma once
#include "MTNet/TcpClient.h"
#include "../JsonUtility.h"
#include <thread>
#include <mutex>
#include <queue>

namespace Network
{
	class PIIO
	{
	public:
		/// <summary>
		/// LED ��Ԃ̎��
		/// </summary>
		enum LED_STATUS : int32_t
		{
			LEDS_OTHER = -1,  // ���̑�
			LEDS_NORMAL = 1,  // �ӂ�
			LEDS_WARNNING = 2,  // �댯�ł�!
			LEDS_CLEAR = 3,  // �N���A���߂łƂ�
		};

		PIIO(const mtnet::IPEndPoint& _localIPEP);
		~PIIO();

		/// <summary>
		/// ���Y�p�C�T�[�o�Ƃ̐ڑ����J�n����
		/// </summary>
		/// <param name="_serverIPEP">�T�[�o��IP�G���h�|�C���g</param>
		void Start(const mtnet::IPEndPoint& _serverIPEP);

		/// <summary>
		/// �t�@������𑗐M����
		/// </summary>
		/// <param name="_rate">��]������ 0.0f ~ 1.0f</param>
		void SendFan(const float _rate);
		/// <summary>
		/// LED��ԑ���𑗐M����
		/// </summary>
		/// <param name="_status">���</param>
		void SendLED(const LED_STATUS _status);

		/// <summary>
		/// ���Y�p�C�T�[�o�Ƃ̐ڑ���ؒf�E�X�g�b�v����
		/// </summary>
		void Stop();
	private:
		/// <summary>
		/// �T�[�o��json�f�[�^�𑗐M����
		/// </summary>
		/// <param name="_json">json�f�[�^</param>
		void SendJson(const json& _json);

		/// <summary>
		/// �r������ň��S�ɃL���[���󂩊m�F����
		/// </summary>
		/// <returns>�L���[����ł��� true / false</returns>
		bool IsEmptySendQueue();

	private:
		bool isRunning_;  // �ʐM�������Ă��邩

		std::thread sessionThread_;  // �T�[�o�Ƃ̔񓯊�����
		mtnet::TcpClient client_;  // TCP �ʐM������{��
		std::mutex sendQueueMutex_;  // ���M�L���[�ւ̓ǂݏ����r������p
		std::queue<std::string> sendQueue_;  // ���M�L���[
		bool isStopped_;  // ��~���߂��łĂ��邩�̃t���O
		std::mutex isStoppedMutex_;  // ��~���߃t���O�̔r������p
	};
}
