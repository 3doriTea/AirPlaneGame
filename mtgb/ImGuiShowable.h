#pragma once
#include <vector>
#include <queue>
#include <string>
#include "TypeRegistry.h"
#include "ISystem.h"
#include <type_traits>
#include <any>
#include <unordered_map>
#include <typeindex>
#include "WindowContext.h"
#include <functional>
#include "MTStringUtility.h"
namespace mtgb
{
	enum class ShowType
	{
		Inspector,
		GameView,
		None
	};
	
	class ImGuiShowable
	{
		friend class ImGuiShowManager;
	public:
		ImGuiShowable();
		ImGuiShowable(ShowType _showType);
		ImGuiShowable(const std::string& _name,ShowType _showType);
		virtual ~ImGuiShowable();

		virtual void ShowImGui();
	protected:
		std::string displayName_;
		ShowType show_;
	};
	class ImGuiShowManager final
	{

	public:
		static ImGuiShowManager& Instance()
		{
			static ImGuiShowManager instance;
			return instance;
		}
		
		void Update();
		void ShowAll(ShowType show);
		template<typename T>
		void Show(T* target, const std::string& name,ShowType show = ShowType::Inspector);
		void Register(ImGuiShowable* obj);
		void Unregister(ImGuiShowable* obj);
	private:
		const mtgb::WindowContext mainWindow_ = WindowContext::First;
		std::vector<ImGuiShowable*> showableObjs_;
		void PushShowFunc(std::function<void()> func, ShowType show);
		std::queue<std::function<void()>> inspectorShowList_;
		std::queue<std::function<void()>> gameViewShowList_;
	};
	
	template<typename T>
	inline void ImGuiShowManager::Show(T* target, const std::string& name, ShowType show)
	{
		using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
		//PushShowFunc( [=] {proxy->ShowImGui(std::any(target), name); }, show);
		PushShowFunc([=](){TypeRegistry::Instance().CallFunc<Type>(target, name.c_str()); }, show);
	}

}