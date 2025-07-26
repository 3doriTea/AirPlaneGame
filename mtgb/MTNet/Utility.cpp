#include "Utility.h"
#include "LobbyCommon.h"
#include "TcpListener.h"
#include <wincrypt.h>
#include <random>

#pragma comment(lib, "Crypt32.lib")  // wincrypt�̃��C�u�����̓ǂݍ���

std::string mtnet::GetLobbyCode(const TcpListener& _listener)
{
	Octet ipCode[LOBBY_CODE_BYTE_SIZE]{};

	// ��4byte����IP�A�h���X�̃I�N�e�b�g������
	if (_listener.LOCAL_IP_END_POINT.TryGetOctets(
		&ipCode[0], &ipCode[1], &ipCode[2], &ipCode[3]) == false)
	{
		throw "IP�A�h���X����I�N�e�b�g�̎擾�Ɏ��s";
	}

	// �c��2byte�Ƀ|�[�g�ԍ�������
	PortNumber port{ _listener.LOCAL_IP_END_POINT.GetPortNumber() };
	ipCode[4] = (port >> (8 * 0)) & 0x00ff;
	ipCode[5] = (port >> (8 * 1)) & 0x00ff;

	// base64�������o���o�b�t�@��p��
	char ipCodeBuffer[LOBBY_CODE_STRING_SIZE]{};
	DWORD ipCodeBufferSize{ LOBBY_CODE_STRING_SIZE };

	// �����A�����o��
	BOOL succeed = CryptBinaryToString(
		ipCode,
		LOBBY_CODE_BYTE_SIZE,
		CRYPT_STRING_BASE64,
		ipCodeBuffer,
		&ipCodeBufferSize);

	if (succeed == FALSE)
	{
		throw "base64�ւ̕ϊ��Ɏ��s";
	}

	return std::string{ ipCodeBuffer };
}

void mtnet::GenerateChallengeCode(
	char* _out,
	const size_t& _size)
{
	// �����Ƃ��̐ݒ�

	std::random_device seedGenerate{};
	std::mt19937 randomEngine{ seedGenerate() };
	std::uniform_int_distribution<int> distribution{ INT8_MIN, INT8_MAX };

	// �����Z�b�g���Ă���
	for (int i = 0; i < _size; i++)
	{
		_out[i] = static_cast<int8_t>(distribution(randomEngine));
	}
}
