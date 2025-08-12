#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include "IncludingInput.h"
#include <wrl/client.h> // �ǉ�
#include <functional>
#include <vector>
#include <tuple>
#include <set>
#include <guiddef.h>
#include <map>
#include "Timer.h"
#include "InputConfig.h"
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")

typedef struct HWND__* HWND;
using Microsoft::WRL::ComPtr; // �ǉ�



namespace mtgb
{
	class InputResource;
	class InputData;

	struct JoystickContext
	{
		TimerHandle timerHandle;
		HRESULT lastResult;
		ComPtr<IDirectInputDevice8> device;
		JoystickContext() = default;
		JoystickContext(IDirectInputDevice8* _device);
	};

	/// <summary>
	/// �W���C�X�e�B�b�N�̊��蓖�ėv��
	/// </summary>
	struct JoystickReservation
	{
		HWND hWnd;
		InputConfig config;
		std::function<void(ComPtr<IDirectInputDevice8>, GUID)> onAssign;
	};

	class Input : public ISystem
	{
	public:
		Input();
		~Input();

		void Initialize() override;
		void Update() override;
		void UpdateKeyDevice();
		void UodateMouseDevice();
		void UpdateJoystickDevice();

		void Release();

		/// <summary>
		/// �}�E�X�̍��W�f�[�^���X�V����
		/// </summary>
		/// <param name="_x">���W x</param>
		/// <param name="_y">���W y</param>
		void UpdateMousePositionData(const int32_t _x, const int32_t _y);

		void CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice);
		void CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice);
		/// <summary>
		/// ���͏�Ԃ��擾����Ώۂ̃f�o�C�X��؂�ւ���
		/// </summary>
		/// <param name="_pJoystickDevice">�؂�ւ��Ώۂ̃L�[�{�[�h�f�o�C�X</param>
		void ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice);

		void SetJoystickGuid(GUID _guid);

		/// <summary>
		/// ���͏�Ԃ��擾����Ώۂ̃f�o�C�X��؂�ւ���
		/// </summary>
		/// <param name="_pJoystickDevice">�؂�ւ��Ώۂ̃}�E�X�f�o�C�X</param>
		void ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice);
		/// <summary>
		/// ���͏�Ԃ��i�[����Ώۂ�؂�Ԃ�
		/// </summary>
		/// <param name="_pJoystickDevice">�؂�ւ��Ώۂ̓��͏�Ԃ��i�[���镨</param>
		void ChangeInputData(InputData* _pInputData);
		/// <summary>
		/// ���͏�Ԃ��擾����Ώۂ̃f�o�C�X��؂�ւ���
		/// </summary>
		/// <param name="_pJoystickDevice">�؂�ւ��Ώۂ̃W���C�X�e�B�b�N�f�o�C�X</param>
		void ChangeJoystickDevice(ComPtr<IDirectInputDevice8> _pJoystickDevice);

		/// <summary>
		/// �ڑ�����Ă���W���C�X�e�B�b�N�̗񋓁A�\��f�o�C�X�ւ̊��蓖�Ă��s��
		/// </summary>
		void EnumJoystick();

		/// <summary>
		/// �W���C�X�e�B�b�N���ڑ����ꂽ�ꍇ�Ɋ��蓖�Ă���悤�\�񂷂�
		/// �撅���Ŋ��蓖�Ă��܂�
		/// </summary>
		/// <param name="_pJoystickDevice">���蓖�Ċ�]�̃f�o�C�X</param>
		void RequestJoystickDevice(JoystickReservation* _reservation);

		/// <summary>
		/// �ڑ�����Ă���W���C�X�e�B�b�N�����蓖�ė\�񂵂Ă�f�o�C�X�Ɋ��蓖�Ă�
		/// </summary>
		/// <param name="_pJoystickDevice"></param>
		void AssignJoystick(IDirectInputDevice8* _pJoystickDevice);

		/// <summary>
		/// �o�^���ꂽ�W���C�X�e�B�b�N����������
		/// </summary>
		/// <param name="_guid">�o�^��������GUID</param>
		/// <returns></returns>
		void UnregisterJoystickGuid(GUID _guid);

		/// <summary>
		/// ���蓖�Ă�ꂽ�W���C�X�e�B�b�N��GUID��o�^����
		/// </summary>
		/// <param name="_guid">�o�^����W���C�X�e�B�b�N��GUID</param>
		/// <returns>�o�^�ς݂̏ꍇ��false��Ԃ�</returns>
		bool RegisterJoystickGuid(GUID _guid);

		/// <summary>
		/// �擾���o��ݒ肷��
		/// </summary>
		void SetAcquireInterval(GUID _guid, ComPtr<IDirectInputDevice8> _device);
		/// <summary>
		/// ���蓖�ė\�񂪂���Ă��Ȃ����ۂ�
		/// </summary>
		/// <returns>/returns>
		bool IsNotSubscribed();

		std::string ConvertHResultToMessage(HRESULT hr) const;

		HRESULT UpdateJoystickState(GUID guid);

		const std::string GetJoystickStatusMessage(GUID guid) const;
		bool IsJoystickConnected(GUID guid) const;
		bool IsJoystickAssigned(GUID guid) const;

	private:
		void AcquireJoystick(ComPtr<IDirectInputDevice8> _pJoystickDevice);
		GUID GetDeviceGuid(ComPtr<IDirectInputDevice8> _pInputDevice);
		void SetProperty(ComPtr<IDirectInputDevice8> _pJoystickDevice, InputConfig _inputConfig);
		InputData* pInputData_;				 // ���͂̏��
		ComPtr<IDirectInput8> pDirectInput_;        // Direct Input �{��k
		ComPtr<IDirectInputDevice8> pKeyDevice_;    // �L�[�f�o�C�X
		ComPtr<IDirectInputDevice8> pMouseDevice_;  // �}�E�X�f�o�C�X
		ComPtr<IDirectInputDevice8> pJoystickDevice_;  // �W���C�X�e�B�b�N�f�o�C�X
		 
		std::vector<JoystickReservation*> requestedJoystickDevices_;//���蓖�ė\�񂳂ꂽ�W���C�X�e�B�b�N�f�o�C�X
		std::set<GUID> assignedJoystickGuids_;//���Ɋ��蓖�čς݂̃W���C�X�e�B�b�N
		
		std::map<GUID, JoystickContext> joystickContext_;
		GUID currJoystickGuid_;
	};
	
}
