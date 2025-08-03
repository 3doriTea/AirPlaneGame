#pragma once
#include <map>
#include <vector>
#include <typeindex>

#include "ReleaseUtility.h"

namespace mtgb
{
    class WindowContextResource;
    /// <summary>
    /// ���\�[�X����Z�߂ɂ����\����
    /// </summary>
    struct ResourceCollection
    {
        std::map<std::type_index, WindowContextResource*> resourceCollection_;//���\�[�X�Q
        std::vector<std::type_index> insertionOrder_;//�}����

        ResourceCollection() = default;
        ResourceCollection(const ResourceCollection& other);
        ResourceCollection(ResourceCollection&& other) noexcept;
        ~ResourceCollection();

        ResourceCollection operator=(const ResourceCollection& other);
        WindowContextResource*& operator[](const std::type_index& key);
        const WindowContextResource* operator[](const std::type_index& key) const;

        /// <summary>
        /// �f�B�[�v�R�s�[����
        /// </summary>
        /// <param name="other">�f�B�[�v�R�s�[����郊�\�[�X�Q</param>
        void Copy(const ResourceCollection& other);

        /// <summary>
        /// �f�B�[�v�R�s�[�������̂�Ԃ�
        /// </summary>
        /// <returns>�f�B�[�v�R�s�[���ꂽ���\�[�X�Q</returns>
        ResourceCollection Clone() const;
        void Release();

        /// <summary>
        /// �w�肳�ꂽ���\�[�X��Ԃ�
        /// </summary>
        /// <typeparam name="Type">���\�[�X�̌^</typeparam>
        /// <returns>�w�肳�ꂽ���\�[�X�̎Q��</returns>
        template<typename Type>
        Type& Get()
        {
            auto itr = resourceCollection_.find(typeid(Type));
            assert(itr != resourceCollection_.end() && "�w�肳�ꂽ�^�̃��\�[�X��������܂���");
            return *dynamic_cast<Type*>(itr->second);
        }

        // ��const�o�[�W����
        auto begin() { return resourceCollection_.begin(); }
        auto end() { return resourceCollection_.end(); }

        // const�o�[�W����
        auto begin() const { return resourceCollection_.begin(); }
        auto end() const { return resourceCollection_.end(); }

        /// <summary>
        /// �}�����Ń��\�[�X�Ɉ����̊֐������s
        /// </summary>
        /// <typeparam name="Func">�֐��̌^</typeparam>
        /// <param name="func">���s����֐�</param>
        template<typename Func>
        void ForEachInOrder(Func&& func)
        {
            for (const auto& key : insertionOrder_)
            {
                func(key, resourceCollection_[key]);
            }

        }
    };
}