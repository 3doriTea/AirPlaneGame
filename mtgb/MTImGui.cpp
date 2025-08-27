#include "MTImGui.h"
#include "Transform.h"
#include "Vector3.h"
#include "ImGuiRenderer.h"
#include "../ImGui/imgui.h"
#include "../ImGui/ImGuizmo.h"
#include "SceneSystem.h"
#include "RectContainsInfo.h"
#include "RectDetector.h"
void mtgb::MTImGui::Initialize()
{
    SetupShowFunc();
    Game::System<SceneSystem>().OnMove([]() 
        {
            MTImGui::Instance().showableObjs_.clear();
        });
}

void mtgb::MTImGui::Update()
{
    
    updatingImGuiShowable_ = true;

    //ImGui::BeginChild("left")
    for (ImGuiShowable* obj : showableObjs_)
    {
        DirectShow([=]()
            {
                ImGui::PushID(obj);

                obj->ShowImGui();

                ImGui::PopID();

            }, obj->displayName_, obj->show_);
    }

    updatingImGuiShowable_ = false;
}
void mtgb::MTImGui::SetupShowFunc()
{
    
    using RegisterShowFuncHolder::Set;

    // テンプレートパラメータに型を指定
    // 第一引数に型のポインタ、第二引数に登録する型の名前

    Set<Transform>([](Transform* _target, const char* _name)
        {
            TypeRegistry::Instance().CallFunc(&_target->position, "Position");
            TypeRegistry::Instance().CallFunc(&_target->rotate, "Rotation");
            TypeRegistry::Instance().CallFunc(&_target->scale, "Scale");
        });

    Set<DirectX::XMVECTOR>([](DirectX::XMVECTOR* _target, const char* _name)
        {
            ImGui::InputFloat4(_name, _target->m128_f32);
        });

    Set<RectContainsInfo>([](RectContainsInfo* _target, const char* _name)
        {
            TypeRegistry::Instance().CallFunc(&_target->worldPos, "WorldPos");
            ImGui::Text("ScreenPos (%.3f,%.3f)", _target->screenPos.x, _target->screenPos.y);
        
            ImGui::Text("EntityId : %lld", _target->entityId);
        });

    Set<RectDetector>([](RectDetector* _target, const char* _name)
        {
            for (auto& target : _target->detectedTargets)
            {
                TypeRegistry::Instance().CallFunc(&target, "RectContains:" + target.entityId);
            }
        });
}
void mtgb::MTImGui::DrawRayImpl(const Vector3& _start, const Vector3& _dir, float _thickness)
{
    std::optional<ImVec2> p1 = Game::System<mtgb::ImGuiRenderer>().Manipulator().WorldToImGui(_start);
    std::optional<ImVec2> p2 = Game::System<mtgb::ImGuiRenderer>().Manipulator().WorldToImGui(_start + _dir);

    if (p1 && p2)
    {
        ImGui::GetWindowDrawList()->AddLine(p1.value(), p2.value(), IM_COL32_WHITE, _thickness);
    }
}
void mtgb::MTImGui::DrawLineImpl(const Vector3& _from, const Vector3& _to, float _thickness)
{
    std::optional<ImVec2> p1 = Game::System<mtgb::ImGuiRenderer>().Manipulator().WorldToImGui(_from);
    std::optional<ImVec2> p2 = Game::System<mtgb::ImGuiRenderer>().Manipulator().WorldToImGui(_to);

    if (p1 && p2)
    {
        ImGui::GetWindowDrawList()->AddLine(p1.value(), p2.value(), IM_COL32_WHITE, _thickness);
    }
}
void mtgb::MTImGui::ShowAll(ShowType show)
{
    if (show == ShowType::Inspector)
    {
        static std::string selectedName;
        static std::function<void()> selectedFunc = nullptr;

        bool isSelected = false;
        ImGui::BeginChild("List", ImVec2(200, 0), true);
        while (!inspectorShowList_.empty())
        {
            const std::string& name = inspectorShowList_.front().first;
            auto& func = inspectorShowList_.front().second;

            if (!isSelected)
            {
                isSelected = selectedName == name;
            }

            if (ImGui::Selectable(name.c_str(),selectedName == name))
            {
                isSelected = true;
                selectedName = name;
                selectedFunc = func;
            }

            inspectorShowList_.pop();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("property", ImVec2(0, 0), true);
        if (selectedFunc && isSelected)
        {
            selectedFunc();
        }
        ImGui::EndChild();
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



void mtgb::MTImGui::DirectShow(std::function<void()> func, const std::string& name, ShowType show)
{
    if (show == ShowType::Inspector)
    {
        inspectorShowList_.emplace(name,func);
    }
    else if (show == ShowType::SceneView)
    {
        sceneViewShowList_.push(func);
    }
}


void mtgb::MTImGui::DrawLine(const Vector3& _from, const Vector3& _to, float _thickness)
{
    if (updatingImGuiShowable_)
    {
        DrawLineImpl(_from, _to, _thickness);
    }
    else
    {
        sceneViewShowList_.push([=]() {DrawLineImpl(_from, _to, _thickness); });
    }
}

/// <summary>
/// レイを表示
/// </summary>
/// <param name="_start">レイ始点</param>
/// <param name="_dir">レイの向きと長さ</param>
/// <param name="_thickness">レイの太さ</param>
void mtgb::MTImGui::DrawVec(const Vector3& _start, const Vector3& _vec, float _thickness)
{
    if (updatingImGuiShowable_)
    {
        DrawRayImpl(_start, _vec, _thickness);
    }
    else
    {
        sceneViewShowList_.push([=]() {DrawRayImpl(_start, _vec, _thickness); });
    }
}
