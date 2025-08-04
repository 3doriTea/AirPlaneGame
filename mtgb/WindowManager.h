#pragma once
#include <vector>
#include <map>
#include <string>
#include "ISystem.h"
#include "WindowContext.h"

typedef struct HWND__* HWND;
typedef struct tagMSG MSG;

namespace mtgb
{
	class WindowResource;
	struct WindowConfig
	{
		std::string title;
		std::string className;
		int width;
		int height;
		int x;
		int y;
	};

	class WindowManager : public ISystem
	{
	public:
		WindowManager();
		~WindowManager();

		static HWND CreateWindowContext(WindowContext context);
		void Initialize() override;
		void Update() override;
		void Release();
		
		/// <summary>
		/// WindowContext�ɑΉ�����WindowConfig��o�^
		/// </summary>
		/// <param name="windowContext">�E�B���h�E�̎��ʎq</param>
		/// <param name="config">�E�B���h�E�ݒ�</param>
		void RegisterWindowConfig(WindowContext windowContext, const WindowConfig& config);
		
		/// <summary>
		/// WindowContext����WindowConfig���擾
		/// </summary>
		/// <param name="windowContext">�E�B���h�E�̎��ʎq</param>
		/// <returns>�E�B���h�E�ݒ�</returns>
		static WindowConfig GetWindowConfig(WindowContext windowContext);
		

		static WindowResource& GetWindowResource(WindowContext windowContext);

	private:
		static MSG* pPeekedMessage_;
		static std::map<WindowContext, WindowConfig> windowConfigMap_;
	};
}