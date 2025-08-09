#pragma once
#include <refl.hpp>
#include "../Source/Player.h"
#include "JoystickProxy.h"
#include "../Source/PlayerProxy.h"
#include "TypeRegistry.h"
//#include "TypeRegistryImpl.h"
#include <string>

// ShowFuncëÆê´ïtÇ´Ç≈TypeÇìoò^
REGISTER_TYPE(PlayerProxy)
REGISTER_FIELD(test1)
REGISTER_FIELD(test2)
REGISTER_FIELD(name)
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
REGISTER_FIELD(connectionStatus)
REGISTER_FIELD(assignmentStatus)
REGISTER_FIELD(lastErrorMessage)
REGISTER_END