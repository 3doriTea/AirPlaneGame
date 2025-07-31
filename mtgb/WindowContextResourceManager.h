#pragma once
#include <functional>
#include <map>
#include <cassert>
#include <typeinfo>
#include "ISystem.h"
#include "WindowContextResource.h"
#include "WindowContext.h"
typedef struct HWND__* HWND;

namespace mtgb
{
    // 型を隠蔽するための基底クラス
    class IWindowContextResourceCollection
    {
    public:
        virtual ~IWindowContextResourceCollection() = default;
        virtual void CreateResource(WindowContext windowContext) = 0;
        virtual void ChangeResource(WindowContext windowContext) = 0;
        virtual HWND GetHWND(WindowContext windowContext) = 0;

        template<typename ResourceT>
        virtual ResourceT& Get(WindowContext windowContext) = 0;
    };

    // テンプレート実装
    template<class... Args>
    class WindowContextResourceCollection : public IWindowContextResourceCollection
    {
        friend class WindowContextResourceManager;
    public:

        WindowContextResourceCollection()
        {
			static constexpr bool allAreBaseOfWindowContextResource =
				(std::is_base_of_v<WindowContextResource, Args> && ...);

			static_assert(allAreBaseOfWindowContextResource, "Args...はWindowContextResourceの派生型である必要があります");
		}

        ~WindowContextResourceCollection() 
        {
            resources_.clear(); 
        }

        using ResourceTuple = std::tuple<Args*...>;
        
        void CreateResource(WindowContext windowContext) override
        {
            ResourceTuple tuple{};
            InitializeSequentially(tuple, windowContext, std::index_sequence_for<Args...>{});
            resources_[windowContext] = std::move(tuple);
        }

        void ChangeResource(WindowContext windowContext) override
        {
            auto itr = resources_.find(windowContext);
            if (itr != resources_.end()) {
                std::apply([](auto*... _resource)
                    {
                        (_resource->SetResource(),...);
                    }, itr->second);
            }
        }

        HWND GetHWND(WindowContext windowContext) override
        {
            auto itr = resources_.find(windowContext);
            if (itr != resources_.end()) {
                // WindowResourceからHWNDを取得
                if constexpr ((std::is_same_v<Args, class WindowResource> || ...)) {
                    auto* windowRes = std::get<class WindowResource*>(itr->second);
                    return windowRes->hWnd_;
                }
            }
            return nullptr;
        }

        template<typename ResourceT>
        ResourceT& Get(WindowContext windowContext) override
        {
            auto itr = resources_.find(windowContext);
            assert(itr != resources_.end() && "指定されたWindowContextのリソースが見つかりません");

            static_assert(std::is_base_of_v<WindowContextResource, ResourceT>,
                "WindowContextResourceを継承していないクラスのインスタンスは取得できません。");
            
           return *(std::get<ResourceT*>(itr->second));
        }
        
        /// <summary>
        /// ウィンドウに固有のリソース達
        /// </summary>
        std::map<WindowContext, ResourceTuple> resources_;
        
        template<std::size_t... Is>
        void InitializeSequentially(ResourceTuple& tuple, WindowContext windowContext, std::index_sequence<Is...>)
        {
            (InitializeResource<Is>(tuple, windowContext), ...);
        }
        
        template<std::size_t I>
        void InitializeResource(ResourceTuple& tuple, WindowContext windowContext)
        {
            using ResourceType = std::tuple_element_t<I, std::tuple<Args...>>;
            std::get<I>(tuple) = new ResourceType;
            std::get<I>(tuple)->Initialize(tuple, windowContext);
        }

    };

    class WindowContextResourceManager : public ISystem
    {
    public:
        WindowContextResourceManager();
        void Initialize() override;
        void Update() override;
        
        void Release();
        
        /// <summary>
        /// 型を指定して登録する関数
        /// </summary>
        /// <typeparam name="...Args">指定した順番に初期化される</typeparam>
        template<class... Args>
        void RegisterResourceTypes()
        {
            pCollection_ = new WindowContextResourceCollection<Args...>();
        }
        
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
        inline ResourceT& Get(WindowContext windowContext)
        {
            return pCollection_->Get<ResourceT>(windowContext);
        }

        /// <summary>
        /// WindowContextからHWNDを取得する
        /// </summary>
        /// <param name="windowContext">ウィンドウの識別子</param>
        /// <returns>HWND</returns>
        HWND GetHWND(WindowContext windowContext);
        
    private:
        IWindowContextResourceCollection* pCollection_;
    };

}