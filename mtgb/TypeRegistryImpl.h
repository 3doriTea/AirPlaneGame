#pragma once
#include "TypeRegistry.h"
#include <refl.hpp>
#include <iostream>
#include "../ImGui/imgui.h"
#include "WindowContext.h"
#include "MTAssert.h"
#include "MTStringUtility.h"
#include "DefaultShow.h"

template <typename F>
struct ShowFunc : refl::attr::usage::type
{
private:
	F func;
public:
	constexpr ShowFunc(F _func) : func(_func)
	{
	}
	void operator()(const char* name) const { func(name); }
};

// ShowFunc���쐬����w���p�[�֐�
template<typename F>
constexpr auto make_show_func(F&& f) {
	return ShowFunc<F>(std::forward<F>(f));
}


// TypeRegistry�̃e���v���[�g����
template<typename T>
void TypeRegistry::RegisterType()
{
	std::type_index typeIdx(typeid(T));
	
	showFunctions_[typeIdx] = [&](std::any ptr, const char* name)
		{
			using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
			if constexpr (refl::is_reflectable<Type>())
			{

				T* registerInstance = nullptr;
				if (ptr.type() == typeid(Type*))
				{
					registerInstance = std::any_cast<Type*>(ptr);
				}
				else if (ptr.type() == typeid(const Type*))
				{
					registerInstance = const_cast<Type*>(std::any_cast<const Type*>(ptr));
				}
				massert(registerInstance != nullptr
					&& "RegisterType�Ɏ��s:ptr��nullptr�ł� @TypeRegistry::RegisterType");
				constexpr auto type = refl::reflect<Type>();

				// Check if ShowFunc is present and execute it
				bool showFuncExecuted = false;
				//type.attributes�̊e�����������_���̈����ɓn���Ĉ������
				std::apply([&](auto&&... attrs)
					{
						(
							(
								[&]
								{
									//�����̃C���X�^���X�̌^���擾
									//�{���̌^������m�肽���̂�decay_t�ŏ����Ȓl�^�ɕϊ�
									using AttrType = std::decay_t<decltype(attrs)>;
									//ShowFunc�^�̃C���X�^���X���ۂ�
									if constexpr (refl::trait::is_instance_of_v<ShowFunc, AttrType>)
									{
										attrs(name);
										showFuncExecuted = true;
									}
								}()
									),
							...);
					}, type.attributes);

				if (!showFuncExecuted)
				{
					if (ImGui::CollapsingHeader(name))
					{
						ImGui::PushID(registerInstance);

						//�����o���Ƃɑ���
						refl::util::for_each(type.members, [&](auto&& member)
							{
								// �����o�̎��ۂ̌^���擾�i�|�C���^���ǂ������܂ށj
								using MemberValueType = std::remove_cvref_t<decltype(member(*registerInstance))>;
								
								if constexpr (std::is_pointer_v<MemberValueType>)
								{
									// �|�C���^�^�̏ꍇ�F���̂܂ܓn��
									auto memberValue = member(*registerInstance);
									
									// �������`�F�b�N���ēK�؂ȕ\�����@��I��
									bool hasCustomAttribute = false;

									// �����o�[�̑������擾
									auto memberAttributes = refl::descriptor::get_attributes(member);

									std::apply([&](auto&&... attrs)
										{
											(
												(
													[&] {
														using AttrType = std::decay_t<decltype(attrs)>;
														if constexpr (std::is_base_of_v<refl::attr::usage::any, AttrType>)
														{
															attrs(memberValue, member.name.c_str());
															hasCustomAttribute = true;
														}
													}()
														), ...);
										}, memberAttributes);

									// �J�X�^���������Ȃ��ꍇ�̓f�t�H���g�\��
									if (!hasCustomAttribute) {
										mtgb::DefaultShow(memberValue, member.name.c_str());
									}
								}
								else
								{
									// �l�^�̏ꍇ�F�A�h���X���擾���ēn��
									auto memberPtr = &(member(*registerInstance));
									
									// �������`�F�b�N���ēK�؂ȕ\�����@��I��
									bool hasCustomAttribute = false;

									// �����o�[�̑������擾
									auto memberAttributes = refl::descriptor::get_attributes(member);

									std::apply([&](auto&&... attrs)
										{
											(
												(
													[&] {
														using AttrType = std::decay_t<decltype(attrs)>;
														if constexpr (std::is_base_of_v<refl::attr::usage::any, AttrType>)
														{
														
															attrs(memberPtr, member.name.c_str());
															hasCustomAttribute = true;
														}
													}()
														), ...);
										}, memberAttributes);

									// �J�X�^���������Ȃ��ꍇ�̓f�t�H���g�\��
									if (!hasCustomAttribute) {
										mtgb::DefaultShow(memberPtr, member.name.c_str());
									}
								}
							});

						ImGui::PopID();
					}
				}
			}
			else
			{
				//���t���N�V��������Ă��Ȃ�
				ImGui::Text("%s,NotReflectable", name);	
			}
		};
}

