#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <array>
#include <bitset>

#include "Vector3.h"
#include "Vector2Int.h"

#include "InputKeyCode.h"
#include "InputMouseCode.h"
#include "cmtgb.h"

#include "Input.h"

struct _DIMOUSESTATE;
struct _XINPUT_STATE;

namespace mtgb
{
	class Input;

	class InputData final
	{
		friend Input;

	public:  // Data getter
		static const bool GetKey(const KeyCode _keyCode);
		static const bool GetKeyDown(const KeyCode _keyCode);
		static const bool GetKeyUp(const KeyCode _keyCode);
		
		static const bool GetMouse(const MouseCode _mouseCode);
		static const bool GetMouseDown(const MouseCode _mouseCode);
		static const bool GetMouseUp(const MouseCode _mouseCode);
		
		static const bool GetGamePad(const MouseCode _mouseCode);
		static const bool GetGamePadDown(const MouseCode _mouseCode);
		static const bool GetGamePadUp(const MouseCode _mouseCode);


		static const Vector2Int GetMousePosition();
		static const Vector3 GetMouseMove();

	private:  // Utilities
		/// <summary>
		/// curr��prev��xor���擾
		/// </summary>
		/// <param name="_keyCode">�L�[�R�[�h</param>
		/// <returns>0: ������, 1: ���L��</returns>
		static inline const int KeyXOR(const KeyCode _keyCode)
		{
			return keyStateCurrent_[Index(_keyCode)] ^ keyStatePrevious_[Index(_keyCode)];
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

	private:  // Data
		static const size_t KEY_COUNT{ 256 };             // �L�[�̐�
		static std::bitset<KEY_COUNT> keyStateCurrent_;   // �L�[�̏�Ԍ���
		static std::bitset<KEY_COUNT> keyStatePrevious_;  // �L�[�̏�ԑO��

		static _DIMOUSESTATE mouseStateCurrent_;   // �}�E�X�̏�Ԍ���
		static _DIMOUSESTATE mouseStatePrevious_;  // �}�E�X�̏�ԑO��
		static Vector2Int mousePosition_;          // �}�E�X�J�[�\���̍��W

		static const size_t GAME_PAD_COUNT{ 4 };  // �Q�[���p�b�h�̍ő�ڑ��\��
		static std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStateCurrent_;   // �Q�[���p�b�h�̏�Ԍ���
		static std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStatePrevious_;  // �Q�[���p�b�h�̏�ԑO��

	private:
		InputData() = delete;
		~InputData() = delete;
	};
}
