#pragma once
#include <typeindex>
#include <functional>
#include <unordered_map>
#include <any>


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
	void CallFunc(std::type_index typeIdx, std::any instance, const char* name);
	bool IsRegisteredType(std::type_index typeIdx);
private:
	std::unordered_map<std::type_index, std::function<void(std::any, const char*)>> showFunctions_;
	std::unordered_map<std::type_index, std::function<void(void)>> provisionalRegisterFunc_;

	TypeRegistry();
	TypeRegistry(const TypeRegistry&) = delete;
	TypeRegistry& operator=(const TypeRegistry&) = delete;
};

// É}ÉNÉçíËã`
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