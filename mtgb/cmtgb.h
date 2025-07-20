#pragma once
// MTGameBase Common includes, type alias and constant values

#include <cstdint>
#include "MTAssert.h"
#include "Handlers.h"

namespace mtgb
{
	using EntityId = int64_t;  // エンティティの識別番号
	static const EntityId INVALD_ENTITY{ -1 };  // -1番は無効Id
}
