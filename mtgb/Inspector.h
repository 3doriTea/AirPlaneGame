#pragma once
#include <typeinfo>
#include <unordered_map>
#include <functional>
#include <refl.hpp>
#include <typeindex>
#include <type_traits>
#include <iostream>
#include "../ImGui/imgui.h"
#include <any>
template <typename T>
struct Range : refl::attr::usage::member
{
	T Min;
	T Max;

	Range(T min, T max) : Min(min), Max(max) {}

	template<typename FieldType>
	void operator()(FieldType* instance, const char* name) const {
		ShowRange(instance, name, Min, Max);
	}
};

template <typename F>
struct ShowFunc : refl::attr::usage::type
{
private:
	F func;
public:
	constexpr ShowFunc(F _func) : func(_func)
	{
	}
	void operator()(const char* name) const { func(name); }
};

// ShowFuncを作成するヘルパー関数
template<typename F>
constexpr auto make_show_func(F&& f) {
	return ShowFunc<F>(std::forward<F>(f));
}


template <typename T>
void ShowRange(T* instance, const char* name, T min, T max)
{
	if constexpr (std::is_same_v<T, int>)
	{
		 ImGui::SliderInt(name, instance, min, max);
	}
	else if constexpr (std::is_same_v<T, float>)
	{
		 ImGui::SliderFloat(name, instance, min, max);
	}
	else
	{
		ImGui::Text("Unknown : %s", name);
	}
}

template <typename T>
struct ReadOnly : refl::attr::usage::member
{
	template<typename FieldType>
	void operator()(FieldType* instance, const char* name) const {
		std::cout << "ReadOnly: " << name << " = " << *instance << std::endl;
	}
};



namespace TypeRegistry
{
	template<typename T>
	void RegisterType();


	using ShowFuncType = std::function<void(std::any, const char*)>;
	//using ShowFuncType = std::function<void(void*, const char*)>;

	template<typename T>
	void RegisterFunc(ShowFuncType func);

	template<typename T>
	void ShowInspector(T * instance, const char* name);
	//std::type_info nowRegisteringType;

	template<typename T>
	void DefaultShow(T* value, const char* name);
	std::unordered_map<std::type_index, ShowFuncType> showFunctions_;


};

template<typename T>
void TypeRegistry::RegisterType()
{
	std::type_index typeIdx(typeid(T));
	showFunctions_[typeIdx] = [](std::any ptr, const char* name)
		{
			if constexpr (refl::is_reflectable<T>())
			{
				T* registerInstance = std::any_cast<T*>(ptr);
				constexpr auto type = refl::reflect<T>();

				// Check if ShowFunc is present and execute it
				bool showFuncExecuted = false;
				//type.attributesの各属性をラムダ式の引数に渡して一つずつ処理
				std::apply([&](auto&&... attrs)
					{
						(
							(
								[&]
								{
									//属性のインスタンスの型を取得
									//本来の型特性を知りたいのでdecay_tで純粋な値型に変換
									using AttrType = std::decay_t<decltype(attrs)>;
									//ShowFunc型のインスタンスか否か
									if constexpr (refl::trait::is_instance_of_v<ShowFunc, AttrType>)
									{
										attrs(name);
										showFuncExecuted = true;
									}
								}()
									),
							...);
					}, type.attributes);

				if (!showFuncExecuted)
				{
					refl::util::for_each(type.members, [&](auto&& member)
						{
							auto memberPtr = &(member(*registerInstance));

							// 属性をチェックして適切な表示方法を選択
							bool hasCustomAttribute = false;

							// メンバーの属性を取得
							auto memberAttributes = refl::descriptor::get_attributes(member);

							std::apply([&](auto&&... attrs)
								{
									(
										(
											[&] {
												using AttrType = std::decay_t<decltype(attrs)>;
												if constexpr (std::is_base_of_v<refl::attr::usage::member, AttrType>)
												{
													attrs(memberPtr, member.name.c_str());
													hasCustomAttribute = true;
												}
											}()
												), ...);
								}, memberAttributes);

							// カスタム属性がない場合はデフォルト表示
							if (!hasCustomAttribute) {
								DefaultShow(memberPtr, member.name.c_str());
							}
						});
				}
			}
		};
}

template<typename T>
void TypeRegistry::RegisterFunc(ShowFuncType func)
{
	std::type_index typeIdx(typeid(T));
	showFunctions_[typeIdx] = func;
}

template<typename T>
void TypeRegistry::ShowInspector(T* instance, const char* name)
{
	
	std::type_index typeIdx(typeid(refl::trait::remove_qualifiers_t<T>));
	auto it = showFunctions_.find(typeIdx);

	if (it != showFunctions_.end())
	{
		it->second(std::any(instance), name);
	}
}

//template<typename T>
//void TypeRegistry::ShowInspector(T* instance, const char* name)
//{
//	//refl::trait::remove_qualifiers_t<T>::type
//	std::type_index typeIdx(typeid(T));
//	auto it = showFunctions_.find(typeIdx);
//
//	if (it != showFunctions_.end())
//	{
//		it->second(instance, name);
//	}
//}

template<typename T>
void TypeRegistry::DefaultShow(T* value, const char* name)
{
	if constexpr (std::is_same_v<T, int>) 
	{
		ImGui::InputInt(name, value);
		//std::cout << "Default int: " << name << " = " << *value << std::endl;
	}
	else if constexpr (std::is_same_v<T, float>) 
	{
		ImGui::InputFloat(name, value);
		//std::cout << "Default float: " << name << " = " << *value << std::endl;
	}
	else if constexpr (std::is_same_v<T, bool>)
	{
		ImGui::Checkbox(name, value);
	}
	else {
		ImGui::Text("%s:Unknown",name );
		//std::cout << "Default unknown type: " << name << std::endl;
	}
}


#define REGISTER_TYPE(Type, ...)\
static struct Type##_TypeRegister {\
	Type##_TypeRegister() {TypeRegistry::RegisterType<Type>();}\
	} Type##_TypeRegister_instance;\
REFL_TYPE(Type, __VA_ARGS__)

#define REGISTER_FIELD(MemberName,...)\
REFL_FIELD(MemberName,__VA_ARGS__)

#define REGISTER_MEMBER_FUNC(MemberName,...)\
REFL_FUNC(MemberName,__VA_ARGS__)

#define REGISTER_END REFL_END

#define REGISTER_SHOW_FUNC()



