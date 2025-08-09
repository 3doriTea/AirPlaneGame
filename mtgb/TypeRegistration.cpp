#include "TypeRegistration.h"
#include "TypeRegistryImpl.h"
#include "Reflection.h"

void TypeRegistry::ProvisionalRegister(std::type_index typeIdx, std::function<void(void)> registerFunc)
{
	provisionalRegisterFunc_.emplace(typeIdx, registerFunc);
}

TypeRegistry& TypeRegistry::Instance()
{
	static TypeRegistry instance;
	return instance;
}

void TypeRegistry::Initialize()
{
	for (auto& itr : provisionalRegisterFunc_)
	{
		itr.second();
	}
}

void TypeRegistry::CallFunc(std::type_index typeIdx, std::any instance, const char* name)
{
	const auto& itr = showFunctions_.find(typeIdx);
	if (itr != showFunctions_.end())
	{
		itr->second(instance, name);
	}
}
bool TypeRegistry::IsRegisteredType(std::type_index typeIdx)
{
	return showFunctions_.contains(typeIdx);
}
TypeRegistry::TypeRegistry()
{
}