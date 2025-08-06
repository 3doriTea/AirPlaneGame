#include <array>
#include "JoystickProxy.h"
#include <iterator>
#include "../ImGui/imgui.h"

mtgb::JoystickProxy::JoystickProxy(const DIJOYSTATE& js) : lX(js.lX), lY(js.lY), lZ(js.lZ),
lRx(js.lRx), lRy(js.lRy), lRz(js.lRz) {
	std::copy(std::begin(js.rglSlider), std::end(js.rglSlider), rglSlider);
	std::copy(std::begin(js.rgdwPOV), std::end(js.rgdwPOV), rgdwPOV);
	std::copy(std::begin(js.rgbButtons), std::end(js.rgbButtons), rgbButtons);
}

mtgb::JoystickProxy mtgb::JoystickProxy::operator=(const DIJOYSTATE& js)
{
    lX = js.lX;
    lY = js.lY;
    lZ = js.lZ;
    lRx = js.lRx;
    lRy = js.lRy;
    lRz = js.lRz;
    std::copy(std::begin(js.rglSlider), std::end(js.rglSlider), rglSlider);
    std::copy(std::begin(js.rgdwPOV), std::end(js.rgdwPOV), rgdwPOV);
    std::copy(std::begin(js.rgbButtons), std::end(js.rgbButtons), rgbButtons);
    return *this;
}




