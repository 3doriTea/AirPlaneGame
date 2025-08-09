#include "ImGuiShowable.h"
#include "../ImGui/imgui.h"
mtgb::ImGuiShowableBase::ImGuiShowableBase(const std::string& name)
    :displayName_{name}
    ,isVisible_{true}
{
}

mtgb::ImGuiShowableBase::~ImGuiShowableBase()
{
}

void mtgb::ImGuiShowableBase::ShowImGui()
{
    ImGui::Text("%s : default show");
}

void mtgb::ImGuiShowableBase::SetVisible(bool visible)
{
    isVisible_ = false;
}

bool mtgb::ImGuiShowableBase::IsVisible()
{
    return isVisible_;
}

void mtgb::ImGuiShowableBase::SetDisplayName(const std::string& name)
{
    displayName_ = name;
}

const std::string& mtgb::ImGuiShowableBase::GetDisplayName() const
{
    return displayName_;
}

void mtgb::ImGuiShowSystem::Register(ImGuiShowableBase* obj)
{
    showList_.push_back(obj);
}

void mtgb::ImGuiShowSystem::Unregister(ImGuiShowableBase* obj)
{
    auto it = std::find(showList_.begin(), showList_.end(), obj);
    if (it != showList_.end()) {
        showList_.erase(it);
    }
}

void mtgb::ImGuiShowSystem::ShowAll()
{
    for (auto* obj : showList_)
    {
        if (obj && obj->IsVisible())
        {
            obj->ShowImGui();
        }
    }
}
