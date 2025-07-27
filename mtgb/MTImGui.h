#pragma once
#include <Windows.h>
#include "ISystem.h"
#include <refl.hpp>
#include <type_traits>
#include <assert.h>
#include <typeinfo>
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
namespace mtgb
{
	/// <summary>
			/// �E�B���h�E����̃��b�Z�[�W����M����ImGui�œ��͂�C�x���g���������邽�߂̃R�[���o�b�N�֐�
			/// </summary>
			/// <param name="hwnd">�E�B���h�E�n���h��</param>
			/// <param name="msg">���b�Z�[�W</param>
			/// <param name="wParam">�p�����[�^</param>
			/// <param name="lParam">�p�����[�^</param>
			/// <returns></returns>
	//IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



	class MTImGui final : public ISystem
	{
	public:
		~MTImGui();
		void Initialize() override;
		void Update() override;
		void BeginFrame();
		void Draw();
		void EndFrame();
		void Release();
		
		template<typename T>
		static void ShowInspector(T* val, const char* name = "");

		static void Show(const char* name, int* val);
		static void Show(const char* name, float* val);
		//static void Show(const char* name, b)
	private:
		//ImGuiIO io;
	};

	/// <summary>
	/// �e���v���[�g�p�����[�^�ɑΉ�����Show�̃I�[�o�[���[�h��T��
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	struct has_custom_show
	{
	private:
		//�Ή�����I�[�o�[���[�h������Ȃ�true_type��Ԃ�
		template<typename U>
		static auto test(int) -> 
			decltype(MTImGui::Show(std::declval<const char*>(), std::declval<U>()),
				std::true_type{});

		//�Ή�����̂��Ȃ������̂�false_type
		template<typename>
		static std::false_type test(...);

		//�]�v�ȏC���q���O��
		using V = refl::trait::remove_qualifiers_t<T>;
	public:
		//�O����V��test���Ă�
		static constexpr bool value = decltype(test<V>(0))::value;
	};

	template<typename T>
	void MTImGui::ShowInspector(T* val, const char* name)
	{
		ImGui::Text(typeid(T).name());
		static_assert(std::is_pointer_v<T*>);
		//static_assert(refl::is_reflectable<T>());
		if constexpr (refl::is_reflectable<T>())
		{
			constexpr refl::descriptor::type_descriptor<T> type = refl::reflect<T>();

			//if(type.bases.size)

			if constexpr (has_custom_show<T*>::value)
			{
				MTImGui::Show(name, val);
			}
			else
			{
				refl::util::for_each(type.members, [&](auto&& member)
					{
						ImGui::Text(name);
						ImGui::Separator();
						MTImGui::ShowInspector(&(member(*val)), member.name.c_str());
					});
			}
		}
		else
		{
			ImGui::Text("Not Reflectable");
		}
	}

}