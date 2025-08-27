#pragma once
#include <typeindex>
#include <functional>
#include <unordered_map>
#include <any>
#include "DefaultShow.h"


class TypeRegistry
{
public:
	template<typename T>
	void RegisterType();
	
	template<typename T>
	void RegisterFunc(std::function<void(std::any, const char*)> func);

	static TypeRegistry& Instance();
	void ProvisionalRegister(std::type_index typeIdx, std::function<void(void)> registerFunc);
	void Initialize();
	template<typename T>
	void CallFunc(T* instance, const char* name);

	void CallFunc(std::type_index typeIdx, std::any instance, const char* name);
	bool IsRegisteredType(std::type_index typeIdx);
private:
	std::unordered_map<std::type_index, std::function<void(std::any, const char*)>> showFunctions_;
	std::unordered_map<std::type_index, std::function<void(void)>> provisionalRegisterFunc_;
	template<typename... Args, typename T>
	bool CheckCustomAttrs(std::tuple<Args...>& attrs, T valPtr, const char* name);
	
	template<typename T>
	void CheckProxyAttrs();

	

	template<typename T>
	bool ShowMemberWithReflection(T memberValue, const char* name);

	TypeRegistry();
	TypeRegistry(const TypeRegistry&) = delete;
	TypeRegistry& operator=(const TypeRegistry&) = delete;
};
template<typename T>
void TypeRegistry::CallFunc(T* instance, const char* name)
{
	const auto& itr = showFunctions_.find(typeid(T));
	if (itr != showFunctions_.end())
	{
		itr->second(std::any(instance),name);
	}
	else
	{
		mtgb::DefaultShow(instance, name);
	}
}

template<typename T>
void TypeRegistry::RegisterFunc(std::function<void(std::any, const char*)> func)
{
	using Type = std::remove_cvref_t<T>;
	std::type_index typeIdx(typeid(Type));
	showFunctions_[typeIdx] = func;
}
namespace RegisterShowFuncHolder
{
	//using ShowFunc = std::function<void(std::any, const char*)>;
	/*RegisterShowFuncHolder(ShowFunc _function):
		function_{ _function }
	{}*/
	//ShowFunc function_;

	/// <summary>
	/// 型に対応したImGuiの表示処理をセットする
	/// </summary>
	/// <typeparam name="Type">表示したい型</typeparam>
	/// <param name="_func">表示したい型を使った表示関数</param>
	template<typename Type>
	void Set(std::function<void(Type* _target, const char* _name)> _func)
	{
		TypeRegistry::Instance().RegisterFunc<Type>([=](std::any target, const char* name)
			{
				_func(std::any_cast<Type*>(target), name);
			});
	}
};

// マクロ定義
#define REGISTER_TYPE(Type, ...) \
struct Type##_TypeRegister{ \
	Type##_TypeRegister(){\
		TypeRegistry::Instance().ProvisionalRegister(typeid(Type),[](){TypeRegistry::Instance().RegisterType<Type>();});\
	}\
};\
static Type##_TypeRegister Type##_instance;\
REFL_TYPE(Type, __VA_ARGS__)

#define REGISTER_FIELD(MemberName,...)\
REFL_FIELD(MemberName,__VA_ARGS__)

#define REGISTER_MEMBER_FUNC(MemberName,...)\
REFL_FUNC(MemberName,__VA_ARGS__)

#define REGISTER_END REFL_END