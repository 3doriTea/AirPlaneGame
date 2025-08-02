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
#include "WindowResource.h"
#include "WindowContextResource.h"
#include "ReleaseUtility.h"
#include "ResourceCollection.h"
typedef struct HWND__* HWND;

namespace mtgb
{
    //using ResourceCollection = std::map<std::type_index, WindowContextResource*>;

    

    class WindowContextResourceManager : public ISystem
    {
    private:
        static WindowContext currentContext_;
        ResourceCollection defResource_;
        std::vector<std::type_index> registeredTypes_;
        std::map<WindowContext, ResourceCollection> collectionMap_;
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
            massert(itr != collectionMap_.end() && "�w�肳�ꂽWindowContext�̃��\�[�X��������܂���");

            static_assert(std::is_base_of_v<WindowContextResource, ResourceT>,
                "WindowContextResource���p�����Ă��Ȃ��N���X�̃C���X�^���X�͎擾�ł��܂���B");

            return itr->second.Get<ResourceT>();
        }

        /// <summary>
        /// WindowContext����HWND���擾����
        /// </summary>
        /// <param name="windowContext">�E�B���h�E�̎��ʎq</param>
        /// <returns>HWND</returns>
        /// <summary>
        /// �^���w�肵�ēo�^����֐�
        /// </summary>
        /// <typeparam name="...Args">�w�肵�����Ԃɏ����������</typeparam>
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

            static_assert(allAreBaseOfWindowContextResource, "Args...��WindowContextResource�̔h���^�ł���K�v������܂�");

            (void)std::initializer_list <int>
            {
                (defResource_[typeid(Args)] = new Args, 0)...
            };
        }
    };
}