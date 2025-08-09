#pragma once
#include <functional>
#include <map>
#include <cassert>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <initializer_list>
#include "ISystem.h"
#include "WindowContext.h"
#include "WindowContextResource.h"
#include "ReleaseUtility.h"
#include "ResourceCollection.h"
typedef struct HWND__* HWND;

namespace mtgb
{

    
    /// <summary>
    /// ウィンドウごとのリソースを管理するクラス
    /// </summary>
    class WindowContextResourceManager : public ISystem
    {
    private:
        WindowContext currentContext_;//現在のウィンドウ
        ResourceCollection defResource_;//複製用のリソース
        std::map<WindowContext, ResourceCollection> collectionMap_;//ウィンドウごとのリソース
    public:
        WindowContextResourceManager();
        ~WindowContextResourceManager();
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
            assert(itr != collectionMap_.end() && "指定されたWindowContextのリソースが見つかりません");

            static_assert(std::is_base_of_v<WindowContextResource, ResourceT>,
                "WindowContextResourceを継承していないクラスのインスタンスは取得できません。");

            return itr->second.Get<ResourceT>();
        }

        

        /// <summary>
        /// 現在有効なウィンドウの識別子を返す
        /// </summary>
        /// <returns>現在のWindowContext</returns>
        WindowContext CurrCtx()
        {
            return currentContext_;
        }

        /// <summary>
        /// 型を指定して登録する関数
        /// </summary>
        /// <typeparam name="...Args">指定した順番に初期化される</typeparam>
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