#pragma once
#include <refl.hpp>
#include "../Source/Player.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "JoystickProxy.h"
#include "Transform.h"
#include "TransformProxy.h"
#include "../Source/PlayerProxy.h"
#include "TypeRegistry.h"
#include "ShowAttributes.h"
//#include "TypeRegistryImpl.h"
#include <string>

REGISTER_TYPE(Quaternion, ShowFunc(XMVECTORF32Show{}))
REGISTER_FIELD(f)
REGISTER_END

REGISTER_TYPE(Vector3,ShowFunc(Vector3Show{}))
REGISTER_FIELD(x)
REGISTER_FIELD(y)
REGISTER_FIELD(z)
REGISTER_END

REGISTER_TYPE(TransformProxy)
REGISTER_FIELD(position_)
REGISTER_FIELD(rotate_)
REGISTER_FIELD(scale_)
REGISTER_END

// ShowFuncëÆê´ïtÇ´Ç≈TypeÇìoò^
REGISTER_TYPE(PlayerProxy)
REGISTER_FIELD(test1)
REGISTER_FIELD(test2)
REGISTER_FIELD(name)
REGISTER_FIELD(pTransformProxy_)
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