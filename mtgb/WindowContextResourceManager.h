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
    // �^���B�����邽�߂̊��N���X
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

    // �e���v���[�g����
    template<class... Args>
    class WindowContextResourceCollection : public IWindowContextResourceCollection
    {
        friend class WindowContextResourceManager;
    public:

        WindowContextResourceCollection()
        {
			static constexpr bool allAreBaseOfWindowContextResource =
				(std::is_base_of_v<WindowContextResource, Args> && ...);

			static_assert(allAreBaseOfWindowContextResource, "Args...��WindowContextResource�̔h���^�ł���K�v������܂�");
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
                // WindowResource����HWND���擾
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
            assert(itr != resources_.end() && "�w�肳�ꂽWindowContext�̃��\�[�X��������܂���");

            static_assert(std::is_base_of_v<WindowContextResource, ResourceT>,
                "WindowContextResource���p�����Ă��Ȃ��N���X�̃C���X�^���X�͎擾�ł��܂���B");
            
           return *(std::get<ResourceT*>(itr->second));
        }
        
        /// <summary>
        /// �E�B���h�E�ɌŗL�̃��\�[�X�B
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
        /// �^���w�肵�ēo�^����֐�
        /// </summary>
        /// <typeparam name="...Args">�w�肵�����Ԃɏ����������</typeparam>
        template<class... Args>
        void RegisterResourceTypes()
        {
            pCollection_ = new WindowContextResourceCollection<Args...>();
        }
        
        /// <summary>
        /// �E�B���h�E���ƂɕK�v�ȃ��\�[�X���쐬
        /// </summary>
        /// <param name="windowContext">�쐬����E�B���h�E�̎��ʎq</param>
        void CreateResource(WindowContext windowContext);

        /// <summary>
        /// ���\�[�X��؂�ւ���
        /// </summary>
        /// <param name="windowContext">�؂�ւ���E�B���h�E�̎��ʎq</param>
        void ChangeResource(WindowContext windowContext);
        
        /// <summary>
        /// ���\�[�X���擾����
        /// </summary>
        /// <typeparam name="ResourceT">���\�[�X�̌^</typeparam>
        /// <param name="windowContext">�擾����E�B���h�E�̎��ʎq</param>
        /// <returns>���\�[�X�̎Q��</returns>
        template<typename ResourceT>
        inline ResourceT& Get(WindowContext windowContext)
        {
            return pCollection_->Get<ResourceT>(windowContext);
        }

        /// <summary>
        /// WindowContext����HWND���擾����
        /// </summary>
        /// <param name="windowContext">�E�B���h�E�̎��ʎq</param>
        /// <returns>HWND</returns>
        HWND GetHWND(WindowContext windowContext);
        
    private:
        IWindowContextResourceCollection* pCollection_;
    };

}