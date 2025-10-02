#include "MTImGui.h"
#include "Transform.h"
#include "Vector3.h"
#include "ImGuiRenderer.h"
#include "../ImGui/imgui.h"
#include "../ImGui/ImGuizmo.h"
#include "SceneSystem.h"
#include "RectContainsInfo.h"
#include "RectDetector.h"
#include <string>
#include <format>
#include "WindowContextUtil.h"
#include "InputResource.h"
#include "WindowResource.h"
#include "Debug.h"
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


    // Settingsウィンドウに表示
    DirectShow([]()
        {
            if (ImGui::Button("SwapWindow"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        WinCtxRes::SwapWindow();
                        // CameraResourceは交換しない
                        Game::System<WinCtxResManager>().SwapResource<InputResource>();
                    });
			}

            if (ImGui::Button("ChangeFullscreenNearestMonitor : FirstWindow"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        Game::System<WindowManager>().ChangeFullScreenStateNearestMonitor(WindowContext::First);
                    });
            }

            if (ImGui::Button("ChangeFullscreenNearestMonitor : SecondWindow"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        Game::System<WindowManager>().ChangeFullScreenStateNearestMonitor(WindowContext::Second);
                    });
            }
		}, "Window", ShowType::Settings);

	DirectShow([]()
		{
            if (ImGui::Button("EnumJoystick"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        Game::System<Input>().EnumJoystick();

                    });
            }
            if (ImGui::Button("SwapInput"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        Game::System<WinCtxResManager>().SwapResource<InputResource>();
                    });

            }
			

		}, "Input", ShowType::Settings);
}
void mtgb::MTImGui::SetWindowOpen(ShowType _showType, bool _flag)
{
    imguiWindowStates_[_showType].isOpen = _flag;
}
void mtgb::MTImGui::SetAllWindowOpen(bool _flag)
{
    for (auto& windowState : imguiWindowStates_)
    {
        windowState.second.isOpen = _flag;
    }
}
void mtgb::MTImGui::ChangeWindowOpen(ShowType _showType)
{
    imguiWindowStates_[_showType].isOpen = !(imguiWindowStates_[_showType].isOpen);
}
void mtgb::MTImGui::ChangeAllWindowOpen()
{
    for (auto& windowState : imguiWindowStates_)
    {
        windowState.second.isOpen = !(windowState.second.isOpen);
    }
}
void mtgb::MTImGui::ShowLog()
{
    using mtgb::Debug;
    const std::list<mtgb::LogEntry>& logs = Game::System<Debug>().GetLog();

    // フィルター用のカテゴリ一覧を作成
    static std::set<std::string> availableCategories;
    static std::string selectedCategory = "All";

    // カテゴリを収集
    availableCategories.clear();
    availableCategories.insert("All");
    for (const auto& log : logs)
    {
        if (!log.category.empty())
        {
            availableCategories.insert(log.category);
        }
    }

    ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();

    imGui.Begin(Debug::GetName().data());

    // カテゴリフィルター用のコンボボックス
    if (ImGui::BeginCombo("Category Filter", selectedCategory.c_str()))
    {
        for (const auto& category : availableCategories)
        {
            bool isSelected = (selectedCategory == category);
            if (ImGui::Selectable(category.c_str(), isSelected))
            {
                selectedCategory = category;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    static int selectedLog = -1;
    int idx = 0;
    int displayIdx = 0;

    for (const mtgb::LogEntry& log : logs)
    {
        // フィルター適用
        if (selectedCategory != "All" && log.category != selectedCategory)
        {
            ++idx;
            continue;
        }

        std::string text = "[" + log.category + "] " + log.msg + " (" + std::to_string(log.count) + ")";

        if (ImGui::Selectable(text.c_str(), selectedLog == idx))
        {
            selectedLog = idx;
        }
        ++idx;
        ++displayIdx;
    }

    // ログの詳細表示
    if (selectedLog >= 0)
    {
        auto it = logs.begin();
        std::advance(it, selectedLog);

        ImGui::Begin("Log Details");
        ImGui::Text("Category: %s", it->category.c_str());
        ImGui::Text("File: %s", it->file.c_str());
        ImGui::Text("Line: %d", it->line);
        ImGui::Text("Function: %s", it->func.c_str());
        ImGui::End();
    }

    imGui.End();

}
mtgb::MTImGui::MTImGui()
{
}
mtgb::MTImGui::~MTImGui()
{
    for (auto queue : showQueues_)
    {
        while (!queue.second.empty())
        {
            queue.second.pop();
        }
    }
    
    showQueues_.clear();
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

    Set<ScreenCoordContainsInfo>([](ScreenCoordContainsInfo* _target, const char* _name)
        {
            TypeRegistry::Instance().CallFunc(&_target->worldPos, "WorldPos");
            ImGui::Text("ScreenPos (%.3f,%.3f)", _target->screenPos.x, _target->screenPos.y);
        
            ImGui::Text("EntityId : %lld", _target->entityId);
        });

    Set<RectDetector>([](RectDetector* _target, const char* _name)
        {
            for (auto& target : _target->GetDetectedTargets())
            {
                TypeRegistry::Instance().CallFunc(&target, "RectContains:" + target.entityId);
            }
        });

    Set<DXGI_ADAPTER_DESC1>([](DXGI_ADAPTER_DESC1* _target, const char* _name)
        {   
            // WCHARの配列を文字列に変換して表示
            char description[256];
            WideCharToMultiByte(CP_UTF8, 0, _target->Description, -1, description, sizeof(description), nullptr, nullptr);
            ImGui::LabelText("Description", "%s", description);
            /*std::string highPart = std::to_string(_target->AdapterLuid.HighPart);
            ImGui::InputText("Adapter LUID:", highPart.data(), highPart.size() + 1, ImGuiInputTextFlags_ReadOnly);
            ImGui::Text("Adapter LUID: %08X-%08X", _target->AdapterLuid.HighPart, _target->AdapterLuid.LowPart);
            ImGui::Text("Flags: 0x%X", _target->Flags);*/
        });

    Set<DXGI_OUTPUT_DESC>([](DXGI_OUTPUT_DESC* _target, const char* _name)
		{
			// WCHARの配列を文字列に変換して表示
			char deviceName[64];
			WideCharToMultiByte(CP_UTF8, 0, _target->DeviceName, -1, deviceName, sizeof(deviceName), nullptr, nullptr);
			ImGui::LabelText("Device Name","%s" ,deviceName);

            ImGui::LabelText("DesktopCoordinates", "(%ld,%ld) - (%ld,%ld)", _target->DesktopCoordinates.left, _target->DesktopCoordinates.top,
                _target->DesktopCoordinates.right, _target->DesktopCoordinates.bottom);
		});
}
void mtgb::MTImGui::ShowListView(ShowType _show)
{
    auto& selectedName = imguiWindowStates_[_show].selectedName;
    auto& queue = showQueues_[_show];

    bool isSelected = false;
    std::function<void()> selectedFunc = nullptr;

    ImGui::BeginChild("List", ImVec2(200, 0), true);

    while (!queue.empty())
    {
        const std::string& name = queue.front().first;
        auto& func = queue.front().second;

        if (!isSelected)
        {
            isSelected = selectedName == name;

            // コピーキャプチャの場合は値が更新されないので、
            // 選択済みの名前と一致していたら関数を更新
            selectedFunc = func;
        }

        // 選択された項目の名前、表示関数を記録
        if (ImGui::Selectable(name.c_str(), selectedName == name))
        {
            isSelected = true;
            imguiWindowStates_[_show].selectedName = name;
            selectedFunc = func;
        }

        queue.pop();
    }
    ImGui::EndChild();

    // Listの横に property表示
    ImGui::SameLine();

    ImGui::BeginChild("property", ImVec2(0, 0), true);
    if (selectedFunc && isSelected)
    {
        selectedFunc();
    }
    ImGui::EndChild();
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
void mtgb::MTImGui::ShowWindow(ShowType _showType)
{
    ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();
    auto& state = imguiWindowStates_[_showType];
    if (!state.isOpen)
    {
        auto& queue = showQueues_[_showType];
        while (!queue.empty())
        {
            queue.pop();
        }
    }

    if (_showType == ShowType::SceneView)
    {
        imGui.Begin(GetName(ShowType::SceneView),&state.isOpen, ImGuiRenderer::WindowFlag::NoMoveWhenHovered);
        
        imGui.UpdateCamera(GetName(ShowType::SceneView));
        imGui.RenderSceneView();
        imGui.SetDrawList();
    }
    else
    {
        imGui.Begin(GetName(_showType), &state.isOpen);
    }

    ExecuteShowQueue(_showType);

    imGui.End();
}
void mtgb::MTImGui::ExecuteShowQueue(ShowType show)
{
    if (show == ShowType::SceneView)
    {
        while (!sceneViewShowList_.empty())
        {
            sceneViewShowList_.front()();
            sceneViewShowList_.pop();
        }
    }
    else
    {
        ShowListView(show);
    }
}

void mtgb::MTImGui::Register(ImGuiShowable* obj)
{
    showableObjs_.push_back(obj);
}

void mtgb::MTImGui::Unregister(ImGuiShowable* obj)
{
    auto it = std::find(showableObjs_.begin(), showableObjs_.end(), obj);
    if (it != showableObjs_.end()) 
    {
        showableObjs_.erase(it);
    }
}



void mtgb::MTImGui::DirectShow(std::function<void()> func, const std::string& name, ShowType show)
{
    if (show == ShowType::SceneView)
    {
        // SceneViewは名前不要
        sceneViewShowList_.push(func);
    }
    else
    {
        showQueues_[show].emplace(name, func);
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
