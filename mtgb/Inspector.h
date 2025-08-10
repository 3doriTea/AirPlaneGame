#pragma once
#include <typeinfo>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <type_traits>
#include <any>
#include <Windows.h>
#include "WindowContext.h"
#include "DefaultShow.h"
#include "TypeRegistry.h"  

using namespace mtgb;

class Inspector
{
public:

	static Inspector& Instance();

	template<typename T>
	void ShowInspector(T* instance, const char* name);
	template<typename T>
	void ShowInspector(const T* instance, const char* name);
private:
	Inspector() {};
	void Show(std::type_index typeIdx,std::any instance,const char* name);
	const static  mtgb::WindowContext mainWindow_ = mtgb::WindowContext::First;
};

template<typename T>
void Inspector::ShowInspector(T* instance, const char* name)
{
	using Type = std::remove_cvref_t<T>;
	std::type_index typeIdx(typeid(Type));
	if (mtgb::CurrContext() != mainWindow_)
	{
		return;
	}

	if (TypeRegistry::Instance().IsRegisteredType(typeIdx))
	{
		TypeRegistry::Instance().CallFunc(typeIdx, std::any(instance), name);
		//Show(typeIdx, std::any(instance), name);
	}
	else
	{
		if (mtgb::CurrContext() != mainWindow_) return;
		mtgb::DefaultShow(instance, name);
	}
}

template<typename T>
void Inspector::ShowInspector(const T* instance, const char* name)
{
	ShowInspector(instance, name);
}

template<typename T>
void TypeRegistry::RegisterFunc(std::function<void(std::any, const char*)> func)
{
	using Type = std::remove_cvref_t<T>;
	std::type_index typeIdx(typeid(Type));
	showFunctions_[typeIdx] = func;
}

