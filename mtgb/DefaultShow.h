#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include "../ImGui/imgui.h"
#include "MTStringUtility.h"

namespace mtgb {
    template<typename T>
    void DefaultShow(T* value, const char* name);
}

// テンプレート実装をインラインで含める
#include "DefaultShow.inl"