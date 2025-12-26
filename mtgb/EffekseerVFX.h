#pragma once

#include <memory>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <DirectXMath.h>
#include "ISystem.h"
#include "Matrix4x4.h"

namespace fs = std::filesystem;

#pragma warning(disable:4100) // 'identifier' : unreferenced formal parameter
#include <EffekseerRendererDX11.h>
#include <Effekseer.h>

#ifdef _DEBUG
#pragma comment(lib, "Effekseerd.lib")
#pragma comment(lib, "EffekseerRendererDX11d.lib")
#else
#pragma comment(lib, "Effekseer.lib")
#pragma comment(lib, "EffekseerRendererDX11.lib")
#endif

namespace mtgb 
{
    //全体で使うEffekseerのマネージャやレンダラなどのデータ
    using RendererRef = EffekseerRendererDX11::RendererRef;

    struct EffectParameters 
    {
        Matrix4x4 worldMat;
        bool isLoop = false;
        bool destoryMe = false;
        float speed = 1.0f;
        Effekseer::Handle handle = 0;
    };
       
    //個別のデータ保持用

    class EffectData
    {
    public:

        EffectData(std::string_view _filePath);
        EffectData(const Effekseer::ManagerRef& _manager, std::string_view _filePath);
        void Load(const Effekseer::ManagerRef& _manager);

        const std::string& GetFilePath() const noexcept;
        const Effekseer::EffectRef& GetEffectRef() const noexcept;
    private:
        const fs::path filePath_;
        Effekseer::EffectRef effectRef_;
    };

    class EffectInstance
    {
    public:
        EffectInstance(const std::shared_ptr<EffectData>& _effectData);

        std::shared_ptr<const EffectData> GetEffectData() const noexcept;

        double elapsedTime = 0;
        std::shared_ptr<EffectParameters> pEffectParameters;
    private:
        const std::shared_ptr<EffectData> pEffectData_;

    };

    class EffectManager : public ISystem
    {
    public:

        virtual ~EffectManager() noexcept
        {
            Release();
        }

        void Initialize();

        void Update() override;

        void Draw();

        void SetCamera();

        /// <summary>
        /// <para> エフェクトのファイルを登録する </para>
        /// <para> 引数で渡した名前がエフェクトのデータ識別子になる </para>
        /// </summary>
        /// <param name="_effectName">エフェクトの名前</param>
        /// <param name="_filePath">エフェクトのファイル(Effekseerが対応する形式)</param>
        void RegisterEffect(std::string_view _effectName, std::string_view _filePath);

        std::shared_ptr<EffectParameters> Play(std::string_view _effectName, const EffectParameters& _effectTransform, bool _isUnique = false);
    private:

        void Release() noexcept
        {
            managerRef_.Reset();
            rendererRef_.Reset();
        }

        float fps_;
        RendererRef rendererRef_;
        Effekseer::ManagerRef managerRef_;
        std::unordered_map<std::string, std::shared_ptr<EffectData>> effectList_;
        std::unordered_multimap<std::string, std::unique_ptr<EffectInstance>> effectInstances_;

    };

}