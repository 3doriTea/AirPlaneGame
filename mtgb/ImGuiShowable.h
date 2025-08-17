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
#include "ShowType.h"
#include <string_view>
namespace mtgb
{
	
	
	/// <summary>
	/// 継承してShowImGuiをオーバライドして使う
	/// これを使わなくてもImGui表示はできるので、別の箇所に分離したいときなどにどうぞ
	/// </summary>
	class ImGuiShowable
	{
		friend class MTImGui;
	public:
		ImGuiShowable();
		ImGuiShowable(ShowType _showType);
		ImGuiShowable(const std::string& _name,ShowType _showType);
		virtual ~ImGuiShowable();

		/// <summary>
		/// 指定した表示キューに積まれる
		/// </summary>
		virtual void ShowImGui();
	protected:
		std::string displayName_;//表示される際の名前
		ShowType show_;//表示したいImGuiWindow
	};
	/// <summary>
	/// ImGuiに表示をする際に使う
	/// </summary>
	class MTImGui final
	{

	public:
		static MTImGui& Instance()
		{
			static MTImGui instance;
			return instance;
		}

		void Initialize();
		void Update();
		
		/// <summary>
		/// 表示キューを一括実行し、クリア
		/// </summary>
		/// <param name="show"></param>
		void ShowAll(ShowType show);

		/// <summary>
		/// 表示キューに積む
		/// 事前に型に結び付けたコールバックを登録する必要あり
		/// </summary>
		/// <typeparam name="T">表示対象の型</typeparam>
		/// <param name="target">表示対象のポインタ</param>
		/// <param name="name">表示対象の名前</param>
		/// <param name="show">表示するImGuiWindow</param>
		template<typename T>
		void TypedShow(T* target, const std::string& name,ShowType show = ShowType::Inspector);
		/// <summary>
		/// ImGuiShowable*インスタンスを登録、毎回ShowImGuiを呼ぶ
		/// ImGuiShowableは自動で登録される
		/// </summary>
		/// <param name="obj"></param>
		void Register(ImGuiShowable* obj);
		/// <summary>
		/// 登録解除
		/// デストラクタで呼ばれる
		/// </summary>
		/// <param name="obj"></param>
		void Unregister(ImGuiShowable* obj);

		/// <summary>
		/// コールバックを表示キューに直接積む
		/// </summary>
		/// <param name="func">コールバック</param>
		/// <param name="show">表示場所</param>
		void DirectShow(std::function<void()> func, ShowType show);

		static constexpr std::string_view GetName(ShowType _showType)
		{
			if (_showType == ShowType::Inspector)
			{
				return "Inspector";
			}
			if (_showType == ShowType::SceneView)
			{
				return "Game View";
			}

			return "None";
		}
	private:
		MTImGui() = default;
		MTImGui(const MTImGui& other) = delete;
		void SetupShowFunc();

		std::vector<ImGuiShowable*> showableObjs_;
		std::queue<std::function<void()>> inspectorShowList_;
		std::queue<std::function<void()>> sceneViewShowList_;
	};
	
	template<typename T>
	inline void MTImGui::TypedShow(T* target, const std::string& name, ShowType show)
	{
		using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
		//PushShowFunc( [=] {proxy->ShowImGui(std::any(target), name); }, show);
		DirectShow([=](){TypeRegistry::Instance().CallFunc<Type>(target, name.c_str()); }, show);
	}
}