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
			/// ウィンドウからのメッセージを受信してImGuiで入力やイベントを処理するためのコールバック関数
			/// </summary>
			/// <param name="hwnd">ウィンドウハンドル</param>
			/// <param name="msg">メッセージ</param>
			/// <param name="wParam">パラメータ</param>
			/// <param name="lParam">パラメータ</param>
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
	/// テンプレートパラメータに対応するShowのオーバーロードを探す
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	struct has_custom_show
	{
	private:
		//対応するオーバーロードがあるならtrue_typeを返す
		template<typename U>
		static auto test(int) -> 
			decltype(MTImGui::Show(std::declval<const char*>(), std::declval<U>()),
				std::true_type{});

		//対応するのがなかったのでfalse_type
		template<typename>
		static std::false_type test(...);

		//余計な修飾子を外す
		using V = refl::trait::remove_qualifiers_t<T>;
	public:
		//外したVでtestを呼ぶ
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