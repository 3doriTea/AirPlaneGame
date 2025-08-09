#pragma once
#include <refl.hpp>
#include <string>
#include <unordered_map>
#include <any>
#include "ReleaseUtility.h"


template<typename Target>
class ReflProxy 
{
protected:
	Target* target_;
public:

	constexpr ReflProxy(Target* target) : target_(target)
	{

	}

	constexpr ReflProxy(const Target* target)
		: target_(target)
	{
	}
	constexpr ~ReflProxy()
	{
		SAFE_DELETE(target_);
	}
};