#pragma once
#include "TypeRegistry.h"
#include <refl.hpp>
#include <iostream>
#include "../ImGui/imgui.h"
#include "WindowContext.h"
#include "MTAssert.h"
#include "MTStringUtility.h"
#include "DefaultShow.h"



template <typename Func>
struct ShowFunc : refl::attr::usage::type
{
private:
	Func func;
public:
	constexpr ShowFunc(Func _func) : func(_func)
	{
	}
	template<typename T>
	void operator()(T value,const char* name) const { func(value,name); }
};

// ShowFuncを作成するヘルパー関数
template<typename F>
constexpr auto make_show_func(F&& f) {
	return ShowFunc<F>(std::forward<F>(f));
}


// TypeRegistryのテンプレート実装
template<typename T>
void TypeRegistry::RegisterType()
{
	std::type_index typeIdx(typeid(T));
	showFunctions_[typeIdx] = [this](std::any ptr, const char* name)
		{
			using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
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
										attrs(registerInstance,name);
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

						//メンバごとに走査
						refl::util::for_each(type.members, [&](auto&& member)
							{
								// メンバの実際の型を取得（ポインタかどうかを含む）
								using MemberValueType = std::remove_cvref_t<decltype(member(*registerInstance))>;
								
								if constexpr (std::is_pointer_v<MemberValueType>)
								{
									// ポインタ型の場合：そのまま渡す
									auto memberValue = member(*registerInstance);
									
									// メンバの型がリフレクションされているかチェック
									if (!this->ShowMemberWithReflection(memberValue, member.name.c_str()))
									{
										// 属性をチェックして適切な表示方法を選択
										bool hasCustomAttribute = false;

										// メンバーの属性を取得
										auto memberAttributes = refl::descriptor::get_attributes(member);



										hasCustomAttribute = this->CheckCustomAttrs(memberAttributes, memberValue, member.name.c_str());

										// カスタム属性がない場合はデフォルト表示
										if (!hasCustomAttribute) {
											mtgb::DefaultShow(memberValue, member.name.c_str());
										}
									}
								}
								else
								{
									// 値型の場合：アドレスを取得して渡す
									auto memberPtr = &(member(*registerInstance));
									
									// メンバの型がリフレクションされているかチェック
									if (!this->ShowMemberWithReflection(memberPtr, member.name.c_str()))
									{
										// 属性をチェックして適切な表示方法を選択
										bool hasCustomAttribute = false;

										// メンバーの属性を取得
										auto memberAttributes = refl::descriptor::get_attributes(member);

										hasCustomAttribute = this->CheckCustomAttrs(memberAttributes, memberPtr, member.name.c_str());

										// カスタム属性がない場合はデフォルト表示
										if (!hasCustomAttribute) {
											mtgb::DefaultShow(memberPtr, member.name.c_str());
										}
									}
								}
							});

						ImGui::PopID();
					}
				}
			}
			else
			{
				//リフレクションされていない
				ImGui::Text("%s,NotReflectable", name);	
			}
		};
}

// メンバの型がリフレクションされているかチェックし、ShowFunc属性があればそれを使用
template<typename T>
bool TypeRegistry::ShowMemberWithReflection(T memberValue, const char* name)
{
	using MemberType = std::remove_pointer_t<std::remove_cvref_t<T>>;
	
	if constexpr (refl::is_reflectable<MemberType>())
	{
		constexpr auto memberType = refl::reflect<MemberType>();
		
		// メンバの型にShowFunc属性があるかチェック
		bool showFuncExecuted = false;
		std::apply([&](auto&&... attrs)
			{
				(
					(
						[&]
						{
							using AttrType = std::decay_t<decltype(attrs)>;
							if constexpr (refl::trait::is_instance_of_v<ShowFunc, AttrType>)
							{
								// ポインタの場合は値を渡し、値型の場合はそのまま渡す
								if constexpr (std::is_pointer_v<T>)
								{
									attrs(memberValue, name);
								}
								else
								{
									attrs(&memberValue, name);
								}
								showFuncExecuted = true;
							}
						}()
							),
					...);
			}, memberType.attributes);
		
		if (showFuncExecuted)
		{
			return true;
		}
		
		// ShowFunc属性がない場合は、TypeRegistryに登録された表示関数を使用
		std::type_index memberTypeIdx(typeid(MemberType));
		
		//TypeRegistryに登録されているかチェック
		if (this->IsRegisteredType(memberTypeIdx))
		{
			if constexpr (std::is_pointer_v<T>)
			{
				this->CallFunc(memberTypeIdx, std::any(memberValue), name);
			}
			else
			{
				this->CallFunc(memberTypeIdx, std::any(&memberValue), name);
			}
			return true;
		}
	}
	
	return false; // リフレクションされていない、または表示関数が見つからない
}

template<typename ...Args, typename T>
bool TypeRegistry::CheckCustomAttrs(std::tuple<Args...>& attrs, T valPtr, const char* name)
{
	bool ret = false;
	std::apply([&](auto&&... attr)
		{
			(
				(
					[&] {
						using AttrType = std::decay_t<decltype(attr)>;
						if constexpr (std::is_base_of_v<refl::attr::usage::any, AttrType>)
						{
							attr(valPtr, name);
							ret = true;
						}
					}()
				), 
			...);
		}, attrs);
	return ret;
}
