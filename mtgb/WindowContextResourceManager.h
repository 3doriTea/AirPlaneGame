#pragma once
#include <functional>
#include <map>
#include "ISystem.h"
#include "WindowContextResource.h"

typedef struct HWND__* HWND;

namespace mtgb
{
    // 型を隠蔽するための基底クラス
    class IWindowContextResourceCollection
    {
    public:
        virtual ~IWindowContextResourceCollection() = default;
        virtual void CreateResource(HWND hWnd) = 0;
        virtual void ChangeResource(HWND hWnd) = 0;
    };

    // テンプレート実装
    template<class... Args>
    class WindowContextResourceCollection : public IWindowContextResourceCollection
    {
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
        /// <param name="hWnd">作成するウィンドウのハンドル</param>
        void CreateResource(HWND hWnd);

        /// <summary>
        /// リソースを切り替える
        /// </summary>
        /// <param name="hWnd">切り替えるウィンドウのハンドル</param>
        void ChangeResource(HWND hWnd);
        
    private:
        IWindowContextResourceCollection* pCollection_;
    };
    
}