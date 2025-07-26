#pragma once
#include "ISystem.h"
#include <typeinfo>
#include <unordered_map>
#include <functional>
#include <refl.hpp>

#define REGISTER_REFL_TYPE(...) {\
	RegisterType</*type(Point)*/>\
	REFL_AUTO(__VA_ARGS__)\
	RegisterFunc(/*func*/)\
}\ 

//#define REGISTER_REFL_TYPE(...) \
//	REFL_AUTO(__VA_ARGS__)\ 

//REGISTER_REFL_TYPE(
//	type(Point),
//	field(x),
//	field(y)
//)

//REFL_AUTO(
//type(Point),
//field(x),
//field(y))

class TypeRegistry
{
public:
	
	template<typename T>
	void RegisterType()
	{
		
	}
	template<typename T>
	void RegisterFunc(ShowFunc func)
	{

	}
private:
	using ShowFunc = std::function<void(void*, const char*)>;
	std::unordered_map<std::type_info, ShowFunc> showFunctions_;

};