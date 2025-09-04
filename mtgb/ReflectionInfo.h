#pragma once
#include <refl.hpp>
#include "../Source/Player.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "JoystickProxy.h"
#include "Transform.h"
#include "TypeRegistry.h"
#include "ShowAttributes.h"
//#include "TypeRegistryImpl.h"
#include <string>

REGISTER_TYPE(Quaternion, ShowFunc(QuaternionSHow{}))
REGISTER_FIELD(f)
REGISTER_END

REGISTER_TYPE(Vector3,ShowFunc(Vector3Show{}))
REGISTER_FIELD(x)
REGISTER_FIELD(y)
REGISTER_FIELD(z)
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
REGISTER_FIELD(deviceName)
REGISTER_FIELD(deviceProductName)
REGISTER_END