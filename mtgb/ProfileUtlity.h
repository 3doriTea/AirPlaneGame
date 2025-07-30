#pragma once
#include "IncludingWindows.h"


namespace mtgb
{
	template<typename T, class ExtendT>
	class IProfile
	{
	public:
		virtual ~IProfile() {}

		virtual ExtendT& Extend() = 0;

		ExtendT& Section(const char* _section) { section_ = _section; return Extend(); }
		ExtendT& Param(const char* _param) { param_ = _param; return Extend(); }
		ExtendT& InitValue(const T _initValue) { initValue_ = _initValue; return Extend(); }
		virtual inline const T Get() const = 0;

		static inline ExtendT Load() { return ExtendT{}; }

	protected:
		const char* section_;
		const char* param_;
		T initValue_;
		static const char* PATH;
	private:
		ExtendT* pExtend_{ nullptr };
	};

	struct ProfileInt : IProfile<int, ProfileInt>
	{
		inline ProfileInt& Extend() override { return *this; }

		inline const int Get() const override
		{
			return GetPrivateProfileInt(section_, param_, initValue_, PATH);
		}
	};

	template<typename T, class ExtendT>
	const char* IProfile<T, ExtendT>::PATH{ "./setup.ini" };
}
