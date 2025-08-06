#pragma once
#include <vector>
#include <string>
#include "Inspector.h"
#include "ISystem.h"

namespace mtgb
{
	/// <summary>
	/// �p���֎~!!
	/// ImGuiShowable���p�����Ă�������
	/// </summary>
	class ImGuiShowableBase
	{
	public:
		ImGuiShowableBase(const std::string& name = "Showable");
		virtual ~ImGuiShowableBase();

		virtual void ShowImGui();

		void SetVisible(bool visible);
		bool IsVisible();

		void SetDisplayName(const std::string& name);
		const std::string& GetDisplayName() const;
	private:
		std::string displayName_;
		bool isVisible_;
	};

	/// <summary>
	/// ������ImGui�\��������C���^�[�t�F�[�X
	/// </summary>
	/// <typeparam name="Derived">�h���N���X�̌^</typeparam>
	template<typename Derived>
	class ImGuiShowable : public ImGuiShowableBase
	{
	public:
		ImGuiShowable(const std::string& name = "Showable");
		virtual ~ImGuiShowable() override;
		/// <summary>
		/// �C�ӂ̕\�����������ꍇ�̓I�[�o���C�h
		/// �I�[�o���C�h���Ȃ���Ύ����ŕ\��������
		/// </summary>
		virtual void ShowImGui() override;
	};


	class ImGuiShowSystem
	{
	public:
		static ImGuiShowSystem& Instance()
		{
			static ImGuiShowSystem instance;
			return instance;
		}
		
		void Register(ImGuiShowableBase* obj);
		void Unregister(ImGuiShowableBase* obj);
		void ShowAll();
	private:
		std::vector<ImGuiShowableBase*> showList_;
	};

	

	template<typename Derived>
	inline ImGuiShowable<Derived>::ImGuiShowable(const std::string& name)
		:ImGuiShowableBase(name)
	{
		ImGuiShowSystem::Instance().Register(this);
	}

	template<typename Derived>
	inline ImGuiShowable<Derived>::~ImGuiShowable()
	{
	}

	template<typename Derived>
	inline void ImGuiShowable<Derived>::ShowImGui()
	{
		TypeRegistry::ShowInspector(static_cast<Derived*>(this), GetDisplayName().c_str());
	}

}