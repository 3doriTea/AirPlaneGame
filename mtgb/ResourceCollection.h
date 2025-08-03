#pragma once
#include <map>
#include <vector>
#include <typeindex>

#include "ReleaseUtility.h"

namespace mtgb
{
    class WindowContextResource;
    /// <summary>
    /// リソースを一纏めにした構造体
    /// </summary>
    struct ResourceCollection
    {
        std::map<std::type_index, WindowContextResource*> resourceCollection_;//リソース群
        std::vector<std::type_index> insertionOrder_;//挿入順

        ResourceCollection() = default;
        ResourceCollection(const ResourceCollection& other);
        ResourceCollection(ResourceCollection&& other) noexcept;
        ~ResourceCollection();

        ResourceCollection operator=(const ResourceCollection& other);
        WindowContextResource*& operator[](const std::type_index& key);
        const WindowContextResource* operator[](const std::type_index& key) const;

        /// <summary>
        /// ディープコピーする
        /// </summary>
        /// <param name="other">ディープコピーされるリソース群</param>
        void Copy(const ResourceCollection& other);

        /// <summary>
        /// ディープコピーしたものを返す
        /// </summary>
        /// <returns>ディープコピーされたリソース群</returns>
        ResourceCollection Clone() const;
        void Release();

        /// <summary>
        /// 指定されたリソースを返す
        /// </summary>
        /// <typeparam name="Type">リソースの型</typeparam>
        /// <returns>指定されたリソースの参照</returns>
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

        /// <summary>
        /// 挿入順でリソースに引数の関数を実行
        /// </summary>
        /// <typeparam name="Func">関数の型</typeparam>
        /// <param name="func">実行する関数</param>
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