#include "MTImGui.h"
#include "Transform.h"
#include "Vector3.h"
#include "ImGuiRenderer.h"
#include "../ImGui/imgui.h"
#include "../ImGui/ImGuizmo.h"

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

void mtgb::MTImGui::DrawLine(const Vector3& _from, const Vector3& _to, float _thickness)
{
    std::optional<ImVec2> p1 = Game::System<mtgb::ImGuiRenderer>().Manipulator().WorldToImGui(_from);
    std::optional<ImVec2> p2 = Game::System<mtgb::ImGuiRenderer>().Manipulator().WorldToImGui(_to);

    if (p1 && p2)
    {
        ImGui::GetWindowDrawList()->AddLine(p1.value(), p2.value(), IM_COL32_WHITE, _thickness);
    }
}
