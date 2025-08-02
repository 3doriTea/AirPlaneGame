#pragma once

#define REFL_FORWARD_DECLARATION()\
namespace refl {\
namespace detail {\
	template <typename, typename, auto> struct member_descriptor;\
}\
}

#define ACCESSIBLE_PRIVATE()\
template<typename,typename,auto> friend struct refl::detail::member_descriptor;