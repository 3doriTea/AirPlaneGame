#pragma once
#include <refl.hpp>
#include "Inspector.h"
#include "../Source/Player.h"
#include "JoystickProxy.h"

using namespace mtgb;

// ShowFuncëÆê´ïtÇ´Ç≈TypeÇìoò^

REGISTER_TYPE(Player)
REGISTER_FIELD(test1)
REGISTER_FIELD(test2)
REGISTER_END


REGISTER_TYPE(JoystickProxy)
REGISTER_FIELD(lX)
REGISTER_FIELD(lY)
REGISTER_FIELD(lZ)
REGISTER_FIELD(lRx)
REGISTER_FIELD(lRy)
REGISTER_FIELD(lRz)
REGISTER_FIELD(rglSlider)
REGISTER_FIELD(rgdwPOV)
REGISTER_FIELD(rgbButtons)
REGISTER_END