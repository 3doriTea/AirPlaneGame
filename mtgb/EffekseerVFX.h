#pragma once

#include <memory>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
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

	/// <summary>
	/// <para> 再生中のエフェクトの制御に使うパラメータをまとめた構造体</para>
	/// <para> ワールド行列を代入することでトランスフォームを変えられる。</para>
	/// <para> destroyMeをtrueにすると更新時に破棄される</para>
	/// </summary>
	struct EffectParameters 
	{
		// ワールド行列
		Matrix4x4 worldMat;
		// ループをするか
		bool isLoop = false;
		// 破棄するか
		bool destroyMe = false;
		// 再生速度
		float speed = 1.0f;
	};
	   
	// 個別のデータ保持用
	class EffectData
	{
	public:
		EffectData(std::string_view _filePath);
		EffectData(const Effekseer::ManagerRef& _manager, std::string_view _filePath);
		/// <summary>
		/// エフェクトを読み込む
		/// </summary>
		/// <param name="_manager"></param>
		void Load(const Effekseer::ManagerRef& _manager);

		/// <summary>
		/// エフェクトのパスを返す
		/// </summary>
		/// <returns></returns>
		std::string GetFilePath() const;
		/// <summary>
		/// エフェクトの参照を返す
		/// </summary>
		/// <returns></returns>
		const Effekseer::EffectRef& GetEffectRef() const;
	private:
		const fs::path filePath_;
		Effekseer::EffectRef effectRef_;
	};

	// エフェクトのインスタンス
	class EffectInstance
	{
	public:
		EffectInstance(const std::shared_ptr<EffectData>& _effectData);

		/// <summary>
		/// インスタンス共有のデータを返す
		/// </summary>
		/// <returns>データのポインタ</returns>
		std::shared_ptr<const EffectData> GetEffectData() const;

		// エフェクトの再生時間
		double elapsedTime_;
		// エフェクト制御用のパラメータ
		std::shared_ptr<EffectParameters> pEffectParameters_;
		// エフェクトのハンドル(エフェクシアが内部で管理してる)
		Effekseer::Handle handle_;
	private:
		// インスタンス共有のデータ
		const std::shared_ptr<EffectData> pEffectData_;

	};

	class EffectManager : public ISystem
	{
	public:

		virtual ~EffectManager() noexcept
		{
			Release();
		}

		void Initialize() override;

		void Update() override;

		void Draw();

		/// <summary>
		/// <para> エフェクトのファイルを登録する </para>
		/// <para> 引数で渡した名前がエフェクトのデータ識別子になる </para>
		/// </summary>
		/// <param name="_effectName">エフェクトの名前</param>
		/// <param name="_filePath">エフェクトのファイル(Effekseerが対応する形式)</param>
		void RegisterEffect(std::string_view _effectName, std::string_view _filePath);

		/// <summary>
		/// <para> エフェクトを再生する </para>
		/// <para> 戻り値のパラメータを使って、エフェクトを制御してください。</para>
		/// <para> </para>
		/// </summary>
		/// <param name="_effectName">エフェクトの名前</param>
		/// <param name="_effectParameters">エフェクトのパラメータ</param>
		/// <returns> パラメータのポインタ </returns>
		std::weak_ptr<EffectParameters> Play(std::string_view _effectName, const EffectParameters& _effectParameters);
	private:

		void SetCamera();
		void Release() noexcept
		{
			managerRef_.Reset();
			rendererRef_.Reset();
		}

		// fps
		float fps_;
		// 描画オブジェクトの参照
		RendererRef rendererRef_;
		// エフェクトのマネージャーの参照
		Effekseer::ManagerRef managerRef_;
		// 個々のエフェクトのデータ
		std::unordered_map<std::string, std::shared_ptr<EffectData>> effectList_;
		// エフェクトインスタンスのプール
		std::vector<std::unique_ptr<EffectInstance>> effectInstances_;
		inline static constexpr size_t kEffectPoolCapacity{ 256 };

	};

}