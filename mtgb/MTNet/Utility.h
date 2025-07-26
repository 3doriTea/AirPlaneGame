#pragma once
#include <string>

namespace mtnet
{
	class TcpListener;

	/// <summary>
	/// ���r�[�R�[�h���擾����
	/// </summary>
	/// <param name="_listener">TcpListener</param>
	/// <returns>���r�[�R�[�h(base64�̕�����)</returns>
	std::string GetLobbyCode(const TcpListener& _listener);

	/// <summary>
	/// �`�������W�R�[�h�𐶐�����
	/// </summary>
	/// <param name="_out">�`�������W�R�[�h�̃|�C���^�n��</param>
	/// <param name="_size">�`�������W�R�[�h�̃o�C�g�T�C�Y</param>
	void GenerateChallengeCode(char* _out, const size_t& _size);
}
