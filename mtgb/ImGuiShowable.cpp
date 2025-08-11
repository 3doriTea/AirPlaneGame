#include "ImGuiShowable.h"
#include "../ImGui/imgui.h"

namespace
{
    UINT defNameCount = 0;
}
mtgb::ImGuiShowableBase::ImGuiShowableBase()
{
    displayName_ = "Showable (" + std::to_string(defNameCount++) + ")";
}
mtgb::ImGuiShowableBase::ImGuiShowableBase(const std::string& name)
    :displayName_{name}
    ,isVisible_{true}
    ,isAuto_{true}
{
}

mtgb::ImGuiShowableBase::~ImGuiShowableBase()
{
}

void mtgb::ImGuiShowableBase::ShowImGui()
{
    ImGui::Text("%s : default show",displayName_.c_str());
}

void mtgb::ImGuiShowableBase::SetVisible(bool visible)
{
    isVisible_ = false;
}

bool mtgb::ImGuiShowableBase::IsVisible()
{
    return isVisible_;
}

bool mtgb::ImGuiShowableBase::IsAuto()
{
    return isAuto_;
}

void mtgb::ImGuiShowableBase::SetDisplayName(const std::string& name)
{
    displayName_ = name;
}

const std::string& mtgb::ImGuiShowableBase::GetDisplayName() const
{
    return displayName_;
}

void mtgb::ImGuiShowSystem::Register(ImGuiShowableBase* obj, Show show)
{
    if (show == Show::Inspector)
    {
        inspectorShowList_.push_back(obj);
    }
    else if (show == Show::GameView)
    {
        gameViewShowList_.push_back(obj);
    }
}

void mtgb::ImGuiShowSystem::Unregister(ImGuiShowableBase* obj,Show show)
{
    if (show == Show::Inspector)
    {
        auto it = std::find(inspectorShowList_.begin(), inspectorShowList_.end(), obj);
        if (it != inspectorShowList_.end()) {
            inspectorShowList_.erase(it);
        }
    }
    else if (show == Show::GameView)
    {
        auto it = std::find(gameViewShowList_.begin(), gameViewShowList_.end(), obj);
        if (it != gameViewShowList_.end()) {
            gameViewShowList_.erase(it);
        }
    }
}

void mtgb::ImGuiShowSystem::ShowAll(Show show)
{
    if (show == Show::Inspector)
    {
        for (auto* obj : inspectorShowList_)
        {
            if (obj && obj->IsAuto())
            {
                obj->ShowImGui();
            }
        }
    }
    else if (show == Show::GameView)
    {
        for (auto* obj : gameViewShowList_)
        {
            if (obj && obj->IsAuto())
            {
                obj->ShowImGui();
            }
        }
    }
}
