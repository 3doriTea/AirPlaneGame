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
typedef struct HWND__* HWND;
namespace mtgb
{
	class Input;
	class InputUtil final
	{
	public:
		static const bool GetKey(const KeyCode _keyCode,HWND _hWnd);
		static const bool GetKeyDown(const KeyCode _keyCode,HWND _hWnd);
		static const bool GetKeyUp(const KeyCode _keyCode,HWND _hWnd);
		static const bool GetMouse(const MouseCode _mouseCode,HWND _hWnd);
		static const bool GetMouseDown(const MouseCode _mouseCode,HWND _hWnd);
		static const bool GetMouseUp(const MouseCode _mouseCode,HWND _hWnd);
		static const bool GetGamePad(const MouseCode _mouseCode,HWND _hWnd);
		static const bool GetGamePadDown(const MouseCode _mouseCode,HWND _hWnd);
		static const bool GetGamePadUp(const MouseCode _mouseCode,HWND _hWnd);


		static const Vector2Int GetMousePosition(HWND _hWnd);
		static const Vector3 GetMouseMove(HWND _hWnd);

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

		static const InputData& InputData(HWND _hWnd);
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
		Vector2Int mousePosition_;          // �}�E�X�J�[�\���̍��W

		static const size_t GAME_PAD_COUNT{ 4 };  // �Q�[���p�b�h�̍ő�ڑ��\��
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStateCurrent_;   // �Q�[���p�b�h�̏�Ԍ���
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStatePrevious_;  // �Q�[���p�b�h�̏�ԑO��

	private:
		InputData() = delete;
		~InputData() = delete;
	};
}
