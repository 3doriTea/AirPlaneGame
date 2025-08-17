#include "ImGuiShowable.h"
#include "../ImGui/imgui.h"
#include "Transform.h"
#include "Game.h"
namespace
{
    uint32_t defNameCount = 0;
    /*constexpr std::string_view WINDOWNAME_GAME_VIEW = "Game View";
    constexpr std::string_view WINDOWNAME_INSPECTOR = "Inspector";
    constexpr std::string_view WINDOWNAME_NONE = "None";*/
}
mtgb::ImGuiShowable::ImGuiShowable()
    :show_{ShowType::Inspector}
{
    MTImGui::Instance().Register(this);
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
    MTImGui::Instance().Register(this);
}

mtgb::ImGuiShowable::ImGuiShowable(const std::string& _name, ShowType _showType)
    :ImGuiShowable(_showType)
{
    displayName_ = _name;
}


mtgb::ImGuiShowable::~ImGuiShowable()
{
    MTImGui::Instance().Unregister(this);
}

void mtgb::MTImGui::Initialize()
{
    SetupShowFunc();
}

void mtgb::MTImGui::Update()
{
	for (ImGuiShowable* obj : showableObjs_)
	{
		DirectShow([=]()
			{
				ImGui::PushID(obj);

				if (ImGui::CollapsingHeader(obj->displayName_.c_str()))
				{
					obj->ShowImGui();
				}

				ImGui::PopID();
			}, obj->show_);
	}
}
void mtgb::MTImGui::SetupShowFunc()
{
    using RegisterShowFuncHolder::Set;

    Set<Transform>([](Transform* _target, const char* _name)
        {
            TypeRegistry::Instance().CallFunc(&_target->position, "Position");
            TypeRegistry::Instance().CallFunc(&_target->rotate, "Rotation");
            TypeRegistry::Instance().CallFunc(&_target->scale, "Scale");
        });
}
void mtgb::MTImGui::ShowAll(ShowType show)
{
    if (show == ShowType::Inspector)
    {
        while (!inspectorShowList_.empty())
        {
            inspectorShowList_.front()();
            inspectorShowList_.pop();
        }
    }
    else if (show == ShowType::SceneView)
    {
        while (!sceneViewShowList_.empty())
        {
            sceneViewShowList_.front()();
            sceneViewShowList_.pop();
        }
    }
}

void mtgb::MTImGui::Register(ImGuiShowable* obj)
{
    showableObjs_.push_back(obj);
}

void mtgb::MTImGui::Unregister(ImGuiShowable* obj)
{
    auto it = std::find(showableObjs_.begin(), showableObjs_.end(), obj);
    if (it != showableObjs_.end()) {
        showableObjs_.erase(it);
    }
}

void mtgb::MTImGui::DirectShow(std::function<void()> func, ShowType show)
{
    if (show == ShowType::Inspector)
    {
        inspectorShowList_.push(func);
    }
    else if (show == ShowType::SceneView)
    {
        sceneViewShowList_.push(func);
    }
}
