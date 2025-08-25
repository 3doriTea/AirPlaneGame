#pragma once

#include <string>

#include "ISystem.h"
#include <any>
#include <unordered_map>
#include <typeindex>
#include "WindowContext.h"
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
	
}