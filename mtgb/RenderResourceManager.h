#pragma once
#include <functional>
#include <map>
#include "ISystem.h"
#include "RenderResource.h"

typedef struct HWND__* HWND;

namespace mtgb
{
    // �^���B�����邽�߂̊��N���X
    class IRenderResourceCollection
    {
    public:
        virtual ~IRenderResourceCollection() = default;
        virtual void CreateResource(HWND hWnd) = 0;
        virtual void ChangeResource(HWND hWnd) = 0;
    };

    // �e���v���[�g����
    template<class... Args>
    class RenderResourceCollection : public IRenderResourceCollection
    {
    public:

        RenderResourceCollection()
        {
			static constexpr bool allAreBaseOfRenderResource =
				(std::is_base_of_v<RenderResource, Args> && ...);

			static_assert(allAreBaseOfRenderResource, "Args...��RenderResource�̔h���^�ł���K�v������܂�");
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
        /// �E�B���h�E�ɌŗL�̃��\�[�X�B
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
        /// �^���w�肵�ēo�^����֐�
        /// </summary>
        /// <typeparam name="...Args">�w�肵�����Ԃɏ����������</typeparam>
        template<class... Args>
        void RegisterResourceTypes()
        {
            pCollection_ = new RenderResourceCollection<Args...>();
        }
        /// <summary>
        /// �`��ɕK�v�ȃ��\�[�X���쐬
        /// </summary>
        /// <param name="hWnd">�쐬����E�B���h�E�̃n���h��</param>
        void CreateRenderResource(HWND hWnd);

        /// <summary>
        /// �`�惊�\�[�X��؂�ւ���
        /// </summary>
        /// <param name="hWnd">�؂�ւ���E�B���h�E�̃n���h��</param>
        void ChangeRenderResource(HWND hWnd);
        
    private:
        IRenderResourceCollection* pCollection_;
    };
    
}