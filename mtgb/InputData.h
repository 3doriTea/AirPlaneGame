#pragma once

#include <array>
#include "IncludingInput.h"
#include <bitset>

#include "Vector3.h"
#include "Vector2Int.h"

#include "InputKeyCode.h"
#include "InputMouseCode.h"
#include "cmtgb.h"
#include "WindowContext.h"

#include "Input.h"


typedef struct HWND__* HWND;
namespace mtgb
{
	enum class Axis
	{
		X,
		Y,
		Z
	};
	class Input;
	class InputUtil final
	{
	public:
		static const bool GetKey(const KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetKeyDown(const KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetKeyUp(const KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetMouse(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetMouseDown(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetMouseUp(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePad(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePadDown(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePadUp(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);

		
		static const float GetAxis(Axis axis,WindowContext _context = mtgb::WindowContext::Both);

		static const Vector2Int GetMousePosition(WindowContext _context);
		static const Vector3 GetMouseMove(WindowContext _context);

	private:  // Utilities
		static const size_t KEY_COUNT{ 256 };             // �L�[�̐�
		/// <summary>
		/// curr��prev��xor���擾
		/// </summary>
		/// <param name="_keyCode">�L�[�R�[�h</param>
		/// <returns>0: ������, 1: ���L��</returns>
		static inline const int KeyXOR(const KeyCode _keyCode,const std::bitset<KEY_COUNT>& _keyStateCurrent, const std::bitset<KEY_COUNT>& _keyStatePrevious)
		{
			return _keyStateCurrent[Index(_keyCode)] ^ _keyStatePrevious[Index(_keyCode)];
		}
		
		/// <summary>
		/// �L�[�R�[�h�\���̗񋓌^���C���f�b�N�X�ɕϊ�
		/// </summary>
		/// <param name="_keyCode">�L�[�R�[�h</param>
		/// <returns>�L�[�z��̃C���f�b�N�X</returns>
		static inline const size_t Index(const KeyCode _keyCode)
		{
			return static_cast<size_t>(_keyCode);
		}

		/// <summary>
		/// ���͏�Ԃ��擾
		/// �ǂ̃E�B���h�E�ł��\��Ȃ��ꍇ��WindowContext::First�̃E�B���h�E���擾�����
		/// </summary>
		/// <param name="_context">�E�B���h�E���w��</param>
		/// <returns></returns>
		static const InputData& GetInput(WindowContext _context);
	};

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

	class InputData final
	{
		friend Input;
		friend InputUtil;
	private:  // Data
		static const size_t KEY_COUNT{ 256 };             // �L�[�̐�
		std::bitset<KEY_COUNT> keyStateCurrent_;   // �L�[�̏�Ԍ���
		std::bitset<KEY_COUNT> keyStatePrevious_;  // �L�[�̏�ԑO��
		
		_DIMOUSESTATE mouseStateCurrent_;   // �}�E�X�̏�Ԍ���
		_DIMOUSESTATE mouseStatePrevious_;  // �}�E�X�̏�ԑO��
		DIJOYSTATE joyStateCurrent_;		// �W���C�X�e�B�b�N�̏�Ԍ���
		DIJOYSTATE joyStatePrevious_;		// �W���C�X�e�B�b�N�̏�Ԍ���

		InputConfig config_;//���͂̎����̐ݒ�

		Vector2Int mousePosition_;          // �}�E�X�J�[�\���̍��W

		static const size_t GAME_PAD_COUNT{ 4 };  // �Q�[���p�b�h�̍ő�ڑ��\��
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStateCurrent_;   // �Q�[���p�b�h�̏�Ԍ���
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStatePrevious_;  // �Q�[���p�b�h�̏�ԑO��

	public:
		//InputData();
		//~InputData();
	};

	
}
