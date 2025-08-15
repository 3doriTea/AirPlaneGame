#include "ImGuiShowable.h"
#include "../ImGui/imgui.h"

namespace
{
    uint32_t defNameCount = 0;
}
mtgb::ImGuiShowable::ImGuiShowable()
    :show_{ShowType::Inspector}
{
    ImGuiShowManager::Instance().Register(this);
    displayName_ = "Default (" + std::to_string(defNameCount++) + ")";
    /*if (defNameCount < UINT32_MAX) {
        displayName_ = "Default (" + std::to_string(defNameCount++) + ")";
    }
    else {
        displayName_ = "Default (Max)";
    }*/
}

void mtgb::ImGuiShowable::ShowImGui()
{

}

mtgb::ImGuiShowable::ImGuiShowable(ShowType _showType)
    :show_{_showType}
{
}

mtgb::ImGuiShowable::ImGuiShowable(const std::string& _name, ShowType _showType)
    :ImGuiShowable(_showType)
{
    displayName_ = _name;
}


mtgb::ImGuiShowable::~ImGuiShowable()
{
}

void mtgb::ImGuiShowManager::Update()
{
    for (ImGuiShowable* obj : showableObjs_)
    {
        ImGui::PushID(obj);
        if (ImGui::CollapsingHeader(obj->displayName_.c_str()))
        {
			PushShowFunc([=]()
				{
                    obj->ShowImGui();
				}, obj->show_);
		}
        ImGui::PopID();
    }   
}

void mtgb::ImGuiShowManager::ShowAll(ShowType show)
{
    if (show == ShowType::Inspector)
    {
        while (!inspectorShowList_.empty())
        {
            inspectorShowList_.front()();
            inspectorShowList_.pop();
        }
    }
    else if (show == ShowType::GameView)
    {
        while (!gameViewShowList_.empty())
        {
            gameViewShowList_.front()();
            gameViewShowList_.pop();
        }
    }
}

void mtgb::ImGuiShowManager::Register(ImGuiShowable* obj)
{
    showableObjs_.push_back(obj);
}

void mtgb::ImGuiShowManager::Unregister(ImGuiShowable* obj)
{
    auto it = std::find(showableObjs_.begin(), showableObjs_.end(), obj);
    if (it != showableObjs_.end()) {
        showableObjs_.erase(it);
    }
}

void mtgb::ImGuiShowManager::PushShowFunc(std::function<void()> func, ShowType show)
{
    if (show == ShowType::Inspector)
    {
        inspectorShowList_.push(func);
    }
    else if (show == ShowType::GameView)
    {
        inspectorShowList_.push(func);
    }
}
