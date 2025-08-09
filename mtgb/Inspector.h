//#pragma once
//#include <typeinfo>
//#include <unordered_map>
//#include <functional>
//#include <refl.hpp>
//#include <typeindex>
//#include <type_traits>
//#include <iostream>
//#include "../ImGui/imgui.h"
//#include <any>
//#include <Windows.h>
//#include "WindowContext.h"
//#include "MTAssert.h"
//#include <stdio.h>
//#include "MTStringUtility.h"
//#include <unordered_set>
//#include "DefaultShow.h"
//#include "TypeRegistration.h"
//
//// TypeRegistrationで定義されたTypeRegistryを使用するため、
//// ここでは属性とヘルパー関数のみを定義
//
//template <typename T>
//struct Range : refl::attr::usage::member
//{
//	T Min;
//	T Max;
//
//	Range(T min, T max) : Min(min), Max(max) {}
//
//	template<typename FieldType>
//	void operator()(FieldType* instance, const char* name) const {
//		ShowRange(instance, name, Min, Max);
//	}
//};
//
//template <typename F>
//struct ShowFunc : refl::attr::usage::type
//{
//private:
//	F func;
//public:
//	constexpr ShowFunc(F _func) : func(_func)
//	{
//	}
//	void operator()(const char* name) const { func(name); }
//};
//
//// ShowFuncを作成するヘルパー関数
//template<typename F>
//constexpr auto make_show_func(F&& f) {
//	return ShowFunc<F>(std::forward<F>(f));
//}
//
//template <typename T>
//void ShowRange(T* instance, const char* name, T min, T max)
//{
//	if constexpr (std::is_same_v<T, int>)
//	{
//		 ImGui::SliderInt(name, instance, min, max);
//	}
//	else if constexpr (std::is_same_v<T, float>)
//	{
//		 ImGui::SliderFloat(name, instance, min, max);
//	}
//	else
//	{
//		ImGui::Text("Unknown : %s", name);
//	}
//}
//
//template <typename T>
//struct ReadOnly : refl::attr::usage::member
//{
//	template<typename FieldType>
//	void operator()(FieldType* instance, const char* name) const {
//		
//	}
//};
//
