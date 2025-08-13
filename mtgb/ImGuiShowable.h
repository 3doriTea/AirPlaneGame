#pragma once
#include <vector>
#include <string>
#include "Inspector.h"
#include "ISystem.h"
#include <type_traits>

namespace mtgb
{
	enum class Show
	{
		Inspector,
		GameView
	};
	/// <summary>
	/// 継承禁止!!
	/// ImGuiShowableを継承してください
	/// </summary>
	class ImGuiShowableBase
	{
	public:
		ImGuiShowableBase();
		ImGuiShowableBase(const std::string& name);
		virtual ~ImGuiShowableBase();

		virtual void ShowImGui();

		void SetVisible(bool visible);
		bool IsVisible();
		bool IsAuto();
		void SetDisplayName(const std::string& name);
		const std::string& GetDisplayName() const;
	protected:
		std::string displayName_;
		bool isVisible_;
		bool isAuto_;
	};

	/// <summary>
	/// 自動でImGui表示をするインターフェース
	/// 既存のクラスをラップ、または自身の型を渡してCRTP
	/// </summary>
	/// <typeparam name="TargetType"></typeparam>
	template<typename TargetType>
	class ImGuiShowable : public ImGuiShowableBase
	{
	public:
		ImGuiShowable(Show show = Show::Inspector);
		ImGuiShowable(const std::string& name,Show show);
		ImGuiShowable(TargetType* derived,Show show);
		
		virtual ~ImGuiShowable() override;
		/// <summary>
		/// 任意の表示をしたい場合はオーバライド
		/// オーバライドしなければ自動で表示方法が選ばれる
		/// </summary>
		virtual void ShowImGui() override;
	protected:
		TargetType* target_;
	};


	class ImGuiShowSystem
	{
	public:
		static ImGuiShowSystem& Instance()
		{
			static ImGuiShowSystem instance;
			return instance;
		}
		
		void Register(ImGuiShowableBase* obj, Show show);
		void Unregister(ImGuiShowableBase* obj,Show show);
		void ShowAll(Show show);
	private:
		std::vector<ImGuiShowableBase*> inspectorShowList_;
		std::vector<ImGuiShowableBase*> gameViewShowList_;
	};
	template<typename TargetType>
	inline ImGuiShowable<TargetType>::ImGuiShowable(Show show)
		:ImGuiShowableBase()
		,target_{nullptr}
	{
		ImGuiShowSystem::Instance().Register(this,show);
	}
	template<typename TargetType>
	inline ImGuiShowable<TargetType>::ImGuiShowable(const std::string& name, Show show)
		:ImGuiShowableBase(name)
		,target_{nullptr}
	{
		ImGuiShowSystem::Instance().Register(this,show);
	}

	template<typename TargetType>
	inline ImGuiShowable<TargetType>::ImGuiShowable(TargetType* target, Show show)
		:ImGuiShowableBase()
		,target_{target}
	{
		ImGuiShowSystem::Instance().Register(this,show);
	}

	template<typename TargetType>
	inline ImGuiShowable<TargetType>::~ImGuiShowable()
	{
	}

	
	template<typename TargetType>
	inline void ImGuiShowable<TargetType>::ShowImGui()
	{
		//ImGuiShowableBase派生クラスの場合はTargetTypeにキャスト
		if constexpr (std::is_base_of_v<ImGuiShowableBase, TargetType>)
		{
			Inspector::Instance().ShowInspector(dynamic_cast<TargetType*>(this), GetDisplayName().c_str());
		}
		//派生していないならそのまま
		else
		{
			Inspector::Instance().ShowInspector(this, GetDisplayName().c_str());
		}
	}

}