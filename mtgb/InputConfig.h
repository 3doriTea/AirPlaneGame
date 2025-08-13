#pragma once
#include "Axis.h"
typedef long LONG;
namespace mtgb
{
	struct InputConfig
	{

		LONG xRange;//x���̒l�͈̔�
		LONG yRange;//y���̒l�͈̔�
		LONG zRange;//z���̒l�͈̔�

		float deadZone;//���͂𖳎�����臒l

		/// <summary>
		/// x,y,z�����ׂĂɔ͈͂̐ݒ�
		/// ���������A�������
		/// </summary>
		/// <param name="_range">�͈͂ƂȂ��Βl</param>
		void SetRange(LONG _range);

		/// <summary>
		/// �w�肵�����ɔ͈͂̐ݒ�
		/// ���������A�������
		/// </summary>
		/// <param name="_range">�͈͂ƂȂ��Βl</param>
		/// <param name="_axis">�ݒ肷�鎲</param>
		void SetRange(LONG _range, Axis _axis);

		/// <summary>
		/// �w�肵���l(����)�Ńf�b�h�]�[����ݒ�
		/// </summary>
		/// <param name="_deadZone">0�`1�͈̔͂Ŏw�肵�Ă�������</param>
		void SetDeadZone(float _deadZone);


		/// <summary>
		/// �f�b�h�]�[����K�p����
		/// </summary>
		/// <param name="value">�K�p����l</param>
		/// <returns>�K�p���ꂽ�l</returns>
		float ApplyDeadZone(const float value) const;
	};
}