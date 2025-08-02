#pragma once
#include "WindowContextResource.h"
#include <functional>
#include <map>
#include <cassert>
#include <vector>
#include <typeinfo>
#include "ISystem.h"
#include "WindowResource.h"
#include "WindowContext.h"
#include <typeindex>
#include <initializer_list>
typedef struct HWND__* HWND;

namespace mtgb
{
    //using ResourceCollection = std::map<std::type_index, WindowContextResource*>;

    struct ResourceCollection
    {
        std::map<std::type_index, WindowContextResource*> resourceCollection_;
        std::vector<std::type_index> insertionOrder_;

        ResourceCollection Clone() const
        {
            ResourceCollection ret;
            for (const auto& [key, ptr] : resourceCollection_)
            {
                ret.resourceCollection_[key] = ptr ? ptr->Clone() : nullptr;
            }
            ret.insertionOrder_ = insertionOrder_;
            return ret;
        }

        WindowContextResource*& operator[](const std::type_index& key)
        {
            if (resourceCollection_.find(key) == resourceCollection_.end())
            {
                insertionOrder_.push_back(key);
            }
            return resourceCollection_[key];
        }
        const WindowContextResource* operator[](const std::type_index& key) const
        {
            auto it = resourceCollection_.find(key);
            return it != resourceCollection_.end() ? it->second : nullptr;
        }

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
#pragma region
    //  // 型を隠蔽するための基底クラス
    //  class IWindowContextResourceCollection
    //  {
    //  public:
    //      virtual ~IWindowContextResourceCollection() = default;
    //      virtual void CreateResource(WindowContext windowContext);
    //      virtual void ChangeResource(WindowContext windowContext);
    //      virtual HWND GetHWND(WindowContext windowContext);


    //      template<typename ResourceT>
    //      ResourceT& Get(WindowContext windowContext);

    //  };

    // 

    //  // テンプレート実装
    //  template<class... Args>
    //  class WindowContextResourceCollection : public IWindowContextResourceCollection
    //  {
    //      friend class WindowContextResourceManager;
    //  public:

    //      WindowContextResourceCollection()
    //      {
          //	static constexpr bool allAreBaseOfWindowContextResource =
          //		(std::is_base_of_v<WindowContextResource, Args> && ...);

          //	static_assert(allAreBaseOfWindowContextResource, "Args...はWindowContextResourceの派生型である必要があります");
          //}

    //      ~WindowContextResourceCollection() 
    //      {
    //          resources_.clear(); 
    //      }

    //      using ResourceTuple = std::tuple<Args*...>;
    //      
    //      void CreateResource(WindowContext windowContext) override
    //      {
    //          ResourceTuple tuple{};
    //          InitializeSequentially(tuple, windowContext, std::index_sequence_for<Args...>{});
    //          resources_[windowContext] = std::move(tuple);
    //      }

    //      void ChangeResource(WindowContext windowContext) override
    //      {
    //          auto itr = resources_.find(windowContext);
    //          if (itr != resources_.end()) {
    //              std::apply([](auto*... _resource)
    //                  {
    //                      (_resource->SetResource(),...);
    //                  }, itr->second);
    //          }
    //      }

    //      HWND GetHWND(WindowContext windowContext) override
    //      {
    //          auto itr = resources_.find(windowContext);
    //          if (itr != resources_.end()) {
    //              // WindowResourceからHWNDを取得
    //              if constexpr ((std::is_same_v<Args, class WindowResource> || ...)) {
    //                  auto* windowRes = std::get<class WindowResource*>(itr->second);
    //                  return windowRes->hWnd_;
    //              }
    //          }
    //          return nullptr;
    //      }

    //      template<typename ResourceT>
    //      ResourceT& Get(WindowContext windowContext) 
    //      {
    //          auto itr = resources_.find(windowContext);
    //          assert(itr != resources_.end() && "指定されたWindowContextのリソースが見つかりません");

    //          static_assert(std::is_base_of_v<WindowContextResource, ResourceT>,
    //              "WindowContextResourceを継承していないクラスのインスタンスは取得できません。");
    //          
    //         return *(std::get<ResourceT*>(itr->second));
    //      }
    //      
    //      /// <summary>
    //      /// ウィンドウに固有のリソース達
    //      /// </summary>
    //      std::map<WindowContext, ResourceTuple> resources_;
    //      
    //      template<std::size_t... Is>
    //      void InitializeSequentially(ResourceTuple& tuple, WindowContext windowContext, std::index_sequence<Is...>)
    //      {
    //          (InitializeResource<Is>(tuple, windowContext), ...);
    //      }
    //      
    //      template<std::size_t I>
    //      void InitializeResource(ResourceTuple& tuple, WindowContext windowContext)
    //      {
    //          using ResourceType = std::tuple_element_t<I, std::tuple<Args...>>;
    //          std::get<I>(tuple) = new ResourceType;
    //          std::get<I>(tuple)->Initialize(tuple, windowContext);
    //      }

    //  };
#pragma endregion
    class WindowContextResourceManager : public ISystem
    {
    private:
        static WindowContext currentContext_;
        ResourceCollection defResource_;
        std::vector<std::type_index> registeredTypes_;
        std::map<WindowContext, ResourceCollection> collectionMap_;
    public:
        WindowContextResourceManager();
        void Initialize() override;
        void Update() override;

        void Release();


        /// <summary>
        /// ウィンドウごとに必要なリソースを作成
        /// </summary>
        /// <param name="windowContext">作成するウィンドウの識別子</param>
        void CreateResource(WindowContext windowContext);

        /// <summary>
        /// リソースを切り替える
        /// </summary>
        /// <param name="windowContext">切り替えるウィンドウの識別子</param>
        void ChangeResource(WindowContext windowContext);

        /// <summary>
        /// リソースを取得する
        /// </summary>
        /// <typeparam name="ResourceT">リソースの型</typeparam>
        /// <param name="windowContext">取得するウィンドウの識別子</param>
        /// <returns>リソースの参照</returns>
        template<typename ResourceT>
        ResourceT& Get(WindowContext windowContext)
        {
            auto itr = collectionMap_.find(windowContext);
            massert(itr != collectionMap_.end() && "指定されたWindowContextのリソースが見つかりません");

            static_assert(std::is_base_of_v<WindowContextResource, ResourceT>,
                "WindowContextResourceを継承していないクラスのインスタンスは取得できません。");

            return itr->second.Get<ResourceT>();
        }

        /// <summary>
        /// WindowContextからHWNDを取得する
        /// </summary>
        /// <param name="windowContext">ウィンドウの識別子</param>
        /// <returns>HWND</returns>
        /// <summary>
        /// 型を指定して登録する関数
        /// </summary>
        /// <typeparam name="...Args">指定した順番に初期化される</typeparam>
        HWND GetHWND(WindowContext windowContext);

        static WindowContext CurrCtx()
        {
            return currentContext_;
        }

        template<class... Args>
        void RegisterResourceTypes()
        {
            static constexpr bool allAreBaseOfWindowContextResource =
                (std::is_base_of_v<WindowContextResource, Args> && ...);

            static_assert(allAreBaseOfWindowContextResource, "Args...はWindowContextResourceの派生型である必要があります");

            (void)std::initializer_list <int>
            {
                (defResource_[typeid(Args)] = new Args, 0)...
            };
        }
    };
}