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
#include <Windows.h>
#include "Game.h"
#include "WindowContextResourceManager.h"
#include "WindowContext.h"
#include "MTAssert.h"
#include <stdio.h>

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
		
	}
};

using ShowFuncType = std::function<void(std::any, const char*)>;

class TypeRegistry
{
public:
	
	template<typename T>
	static void RegisterType();


	//using ShowFuncType = std::function<void(void*, const char*)>;

	template<typename T>
	static void RegisterFunc(ShowFuncType func);

	template<typename T>
	static void ShowInspector(T * instance, const char* name);
	//std::type_info nowRegisteringType;

	template<typename T>
	static void DefaultShow(T* value, const char* name);
	static std::unordered_map<std::type_index, ShowFuncType> showFunctions_;

	const static  mtgb::WindowContext mainWindow_ = mtgb::WindowContext::First;

	static TypeRegistry instance;
private:
	TypeRegistry() = delete;
	TypeRegistry(const TypeRegistry&) = delete;
	TypeRegistry& operator=(const TypeRegistry&) = delete;
};

template<typename T>
void TypeRegistry::RegisterType()
{
	std::type_index typeIdx(typeid(T));
	showFunctions_[typeIdx] = [](std::any ptr, const char* name)
		{
			using Type = refl::trait::remove_qualifiers_t<T>;
			if constexpr (refl::is_reflectable<Type>())
			{
				T* registerInstance = nullptr;
				if (ptr.type() == typeid(Type*))
				{
					registerInstance = std::any_cast<Type*>(ptr);
				}
				else if (ptr.type() == typeid(const Type*))
				{
					registerInstance = const_cast<Type*>(std::any_cast<const Type*>(ptr));
				}
				massert(registerInstance != nullptr
					&& "RegisterTypeに失敗:ptrがnullptrです @TypeRegistry::RegisterType");
				constexpr auto type = refl::reflect<Type>();

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
					if (ImGui::CollapsingHeader(name))
					{
						ImGui::PushID(registerInstance);
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

						ImGui::PopID();
					}
				}
			}
			else
			{
				
				ImGui::Text("%s,not", name);
					
			}
		};
}

template<typename T>
void TypeRegistry::RegisterFunc(ShowFuncType func)
{
	using Type = refl::trait::remove_qualifiers_t<T>;
	std::type_index typeIdx(typeid(Type));
	showFunctions_[typeIdx] = func;
}

template<typename T>
void TypeRegistry::ShowInspector(T* instance, const char* name)
{
	if (mtgb::WindowContextResourceManager::CurrCtx() != mainWindow_)
	{
		return;
	}
	std::type_index typeIdx(typeid(refl::trait::remove_qualifiers_t<T>));
	auto it = showFunctions_.find(typeIdx);

	if (it != showFunctions_.end())
	{
		it->second(std::any(instance), name);
	}
	else
	{
		ImGui::Text("%s,notReflectable", name);
	}
}



template<typename T>
void TypeRegistry::DefaultShow(T* value, const char* name)
{
	using Type = refl::trait::remove_qualifiers_t<T>;
	if constexpr (std::is_array_v<Type>)
	{
		if (ImGui::CollapsingHeader(name))
		{
			ImGui::Indent();

			//要素型取得
			using ElemType = std::remove_extent_t<Type>;
			//0次元目の要素数取得
			constexpr size_t N = std::extent_v<Type>;

			//配列要素ごとに再帰する
			for (size_t i = 0; i < N; ++i)
			{
				std::string elemName = std::string(name) + "[" + std::to_string(i) + "]";
				DefaultShow(static_cast<ElemType*>(&(*value)[i]), elemName.c_str());
			}

			ImGui::Unindent();
		}
	}
	else if constexpr (std::is_same_v<Type, bool>)
	{
		ImGui::Checkbox(name, reinterpret_cast<bool*>(value));
	}
	else if constexpr (std::is_floating_point_v<Type> ) 
	{
		ImGui::InputFloat(name, reinterpret_cast<float*>(value));
	}
	else if constexpr (std::is_same_v<Type,int>)
	{
		ImGui::InputInt(name, reinterpret_cast<int*>(value));
		
	}
	else if constexpr (std::is_same_v<Type, long>)
	{
		ImGui::Text("%s : %4.2ld", name, *value);
		/*int* temp = static_cast<int*>(value);
		if (ImGui::InputInt(name, temp))
		{
			*value = static_cast<long>(*temp);
		}*/
	}
	else if constexpr (std::is_same_v<Type, unsigned long>)
	{
		ImGui::Text("%s : %4.2lo", name, *value);
	}
	else if constexpr (std::is_same_v<Type, unsigned char>)
	{
		ImGui::Text("%s : %4.2hhu", name, *value);
	}
	else if constexpr (std::is_enum_v<Type>)
	{

	}
	else if constexpr (std::is_same_v<Type, std::string>)
	{
		constexpr size_t bufSize = 256;
		char buf[bufSize];

		strncpy_s(buf, value->c_str(), bufSize);
		buf[bufSize - 1] = '\0';

		if (ImGui::InputText(name, buf, bufSize))
		{
			*value = std::string(buf);
		}
	}
	else
	{
		ImGui::Text("%s:Unknown,%s",name,typeid(Type).name() );
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

#define ACCESSIBLE_PRIVATE()\
template<typename,typename,auto> friend struct refl::detail::member_descriptor;

