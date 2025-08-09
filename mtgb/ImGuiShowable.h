#pragma once
#include <vector>
#include <string>
#include "Inspector.h"
#include "ISystem.h"
#include <type_traits>

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
		bool IsAuto();
		void SetDisplayName(const std::string& name);
		const std::string& GetDisplayName() const;
	protected:
		std::string displayName_;
		bool isVisible_;
		bool isAuto_;
	};

	/// <summary>
	/// ������ImGui�\��������C���^�[�t�F�[�X
	/// �����̃N���X�����b�v�A�܂��͎��g�̌^��n����CRTP
	/// </summary>
	/// <typeparam name="TargetType"></typeparam>
	template<typename TargetType>
	class ImGuiShowable : public ImGuiShowableBase
	{
	public:
		ImGuiShowable(const std::string& name = "Showable");
		ImGuiShowable(TargetType* derived);
		
		virtual ~ImGuiShowable() override;
		/// <summary>
		/// �C�ӂ̕\�����������ꍇ�̓I�[�o���C�h
		/// �I�[�o���C�h���Ȃ���Ύ����ŕ\�����@���I�΂��
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
		
		void Register(ImGuiShowableBase* obj);
		void Unregister(ImGuiShowableBase* obj);
		void ShowAll();
	private:
		std::vector<ImGuiShowableBase*> showList_;
	};
	template<typename TargetType>
	inline ImGuiShowable<TargetType>::ImGuiShowable(const std::string& name)
		:ImGuiShowableBase(name),target_{nullptr}
	{
		ImGuiShowSystem::Instance().Register(this);
	}

	template<typename TargetType>
	inline ImGuiShowable<TargetType>::ImGuiShowable(TargetType* target)
		:target_{target}
	{
	}

	template<typename TargetType>
	inline ImGuiShowable<TargetType>::~ImGuiShowable()
	{
	}

	
	template<typename TargetType>
	inline void ImGuiShowable<TargetType>::ShowImGui()
	{
		//ImGuiShowableBase�h���N���X�̏ꍇ��TargetType�ɃL���X�g
		if constexpr (std::is_base_of_v<ImGuiShowableBase, TargetType>)
		{
			Inspector::Instance().ShowInspector(dynamic_cast<TargetType*>(this), GetDisplayName().c_str());
		}
		//�h�����Ă��Ȃ��Ȃ炻�̂܂�
		else
		{
			Inspector::Instance().ShowInspector(this, GetDisplayName().c_str());
		}
	}

}