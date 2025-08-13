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
    /// �E�B���h�E���Ƃ̃��\�[�X���Ǘ�����N���X
    /// </summary>
    class WindowContextResourceManager : public ISystem
    {
    private:
        WindowContext currentContext_;//���݂̃E�B���h�E
        ResourceCollection defResource_;//�����p�̃��\�[�X
        std::map<WindowContext, ResourceCollection> collectionMap_;//�E�B���h�E���Ƃ̃��\�[�X
    public:
        WindowContextResourceManager();
        ~WindowContextResourceManager();
        void Initialize() override;
        void Update() override;

        void Release();


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
        ResourceT& Get(WindowContext windowContext)
        {
            auto itr = collectionMap_.find(windowContext);
            assert(itr != collectionMap_.end() && "�w�肳�ꂽWindowContext�̃��\�[�X��������܂���");

            static_assert(std::is_base_of_v<WindowContextResource, ResourceT>,
                "WindowContextResource���p�����Ă��Ȃ��N���X�̃C���X�^���X�͎擾�ł��܂���B");

            return itr->second.Get<ResourceT>();
        }

        

        /// <summary>
        /// ���ݗL���ȃE�B���h�E�̎��ʎq��Ԃ�
        /// </summary>
        /// <returns>���݂�WindowContext</returns>
        WindowContext CurrCtx()
        {
            return currentContext_;
        }

        /// <summary>
        /// �^���w�肵�ēo�^����֐�
        /// </summary>
        /// <typeparam name="...Args">�w�肵�����Ԃɏ����������</typeparam>
        template<class... Args>
        void RegisterResourceTypes()
        {
            static constexpr bool allAreBaseOfWindowContextResource =
                (std::is_base_of_v<WindowContextResource, Args> && ...);

            static_assert(allAreBaseOfWindowContextResource, "Args...��WindowContextResource�̔h���^�ł���K�v������܂�");

            (void)std::initializer_list <int>
            {
                (defResource_[typeid(Args)] = new Args, 0)...
            };
        }
    };
}