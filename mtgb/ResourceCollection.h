#pragma once
#include <map>
#include <vector>
#include <typeindex>

#include "ReleaseUtility.h"

namespace mtgb
{
    class WindowContextResource;
    struct ResourceCollection
    {
        std::map<std::type_index, WindowContextResource*> resourceCollection_;
        std::vector<std::type_index> insertionOrder_;

        ResourceCollection() = default;
        ResourceCollection(const ResourceCollection& other);
        ResourceCollection(ResourceCollection&& other) noexcept;
        ~ResourceCollection();

        ResourceCollection operator=(const ResourceCollection& other);
        WindowContextResource*& operator[](const std::type_index& key);
        const WindowContextResource* operator[](const std::type_index& key) const;

        void Copy(const ResourceCollection& other);
        ResourceCollection Clone() const;
        void Release();

        template<typename Type>
        Type& Get()
        {
            auto itr = resourceCollection_.find(typeid(Type));
            assert(itr != resourceCollection_.end() && "指定された型のリソースが見つかりません");
            return *dynamic_cast<Type*>(itr->second);
        }

        // 非constバージョン
        auto begin() { return resourceCollection_.begin(); }
        auto end() { return resourceCollection_.end(); }

        // constバージョン
        auto begin() const { return resourceCollection_.begin(); }
        auto end() const { return resourceCollection_.end(); }

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