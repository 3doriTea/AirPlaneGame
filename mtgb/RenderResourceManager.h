#pragma once
#include <functional>
#include <map>
#include "ISystem.h"
#include "RenderResource.h"

typedef struct HWND__* HWND;

namespace mtgb
{
    // 型を隠蔽するための基底クラス
    class IRenderResourceCollection
    {
    public:
        virtual ~IRenderResourceCollection() = default;
        virtual void CreateResource(HWND hWnd) = 0;
        virtual void ChangeResource(HWND hWnd) = 0;
    };

    // テンプレート実装
    template<class... Args>
    class RenderResourceCollection : public IRenderResourceCollection
    {
    public:

        RenderResourceCollection()
        {
			static constexpr bool allAreBaseOfRenderResource =
				(std::is_base_of_v<RenderResource, Args> && ...);

			static_assert(allAreBaseOfRenderResource, "Args...はRenderResourceの派生型である必要があります");
		}

        ~RenderResourceCollection() 
        {
            resources_.clear(); 
        }

        using ResourceTuple = std::tuple<Args*...>;
        
        void CreateResource(HWND hWnd) override
        {
            ResourceTuple tuple{};
            InitializeSequentially(tuple, hWnd, std::index_sequence_for<Args...>{});
            resources_[hWnd] = std::move(tuple);
        }

        void ChangeResource(HWND hWnd) override
        {
            std::apply([this](auto*... _resource)
                {
                    (_resource->SetResource(),...);
                }, resources_[hWnd]);
        }
    private:
        /// <summary>
        /// ウィンドウに固有のリソース達
        /// </summary>
        std::map<HWND, ResourceTuple> resources_;
        
        template<std::size_t... Is>
        void InitializeSequentially(ResourceTuple& tuple, HWND hWnd, std::index_sequence<Is...>)
        {
            (InitializeResource<Is>(tuple, hWnd), ...);
        }
        
        template<std::size_t I>
        void InitializeResource(ResourceTuple& tuple, HWND hWnd)
        {
            using ResourceType = std::tuple_element_t<I, std::tuple<Args...>>;
            std::get<I>(tuple) = new ResourceType;
            std::get<I>(tuple)->Initialize(tuple, hWnd);
        }
    };

    class RenderResourceManager : public ISystem
    {
    public:
        RenderResourceManager();
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
            pCollection_ = new RenderResourceCollection<Args...>();
        }
        /// <summary>
        /// 描画に必要なリソースを作成
        /// </summary>
        /// <param name="hWnd">作成するウィンドウのハンドル</param>
        void CreateRenderResource(HWND hWnd);

        /// <summary>
        /// 描画リソースを切り替える
        /// </summary>
        /// <param name="hWnd">切り替えるウィンドウのハンドル</param>
        void ChangeRenderResource(HWND hWnd);
        
    private:
        IRenderResourceCollection* pCollection_;
    };
    
}