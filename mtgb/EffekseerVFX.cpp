#include "Assert.h"
#include "EffekseerVFX.h"
#include "CameraSystem.h"
#include "GameTime.h"
#include "DirectX11Draw.h"
#include "Screen.h"
#include "Debug.h"
namespace mtgb 
{
	/// <summary>
	/// ゲームで使用するエフェクトを登録
	/// </summary>
	void RegisterEffects()
	{
		mtgb::EffectManager& effectManager = mtgb::Game::System<mtgb::EffectManager>();
		effectManager.RegisterEffect("Explosion", "Effect/Explosion.efkefc");
		effectManager.RegisterEffect("Smoke", "Effect/Smoke.efkefc");
		effectManager.RegisterEffect("Fire", "Effect/Fire.efkefc");
		effectManager.RegisterEffect("MissileShotDown", "Effect/MissileShotDown.efkefc");
	}
	using namespace Effekseer;
	using namespace EffekseerRendererDX11;
	constexpr int MAX_SQUARE{ 8192 };
	Effekseer::Matrix43 CnvMat43(DirectX::XMFLOAT4X4 _mat)
	{
		Effekseer::Matrix43 mat43{};
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				mat43.Value[i][j] = _mat.m[i][j];
			}
		}
		return mat43;
	}

	Effekseer::Matrix44 CnvMat(DirectX::XMFLOAT4X4 _mat)
	{
		Effekseer::Matrix44 out;

		out.Values[0][0] = _mat._11;
		out.Values[1][0] = _mat._12;
		out.Values[2][0] = _mat._13;
		out.Values[3][0] = _mat._14;
		out.Values[0][1] = _mat._21;
		out.Values[1][1] = _mat._22;
		out.Values[2][1] = _mat._23;
		out.Values[3][1] = _mat._24;
		out.Values[0][2] = _mat._31;
		out.Values[1][2] = _mat._32;
		out.Values[2][2] = _mat._33;
		out.Values[3][2] = _mat._34;
		out.Values[0][3] = _mat._41;
		out.Values[1][3] = _mat._42;
		out.Values[2][3] = _mat._43;
		out.Values[3][3] = _mat._44;
		return out;
	}
	Effekseer::Matrix44 CnvMat(const Matrix4x4& _mat)
	{
		Effekseer::Matrix44 out;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				out.Values[i][j] = _mat.r[i].m128_f32[j];
			}
		}
		return out;
	}
	Effekseer::Matrix43 CnvMat43(const Matrix4x4& _mat)
	{
		Effekseer::Matrix43 out;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				out.Value[i][j] = _mat.r[i].m128_f32[j];
			}
		}
		return out;
	}
	DirectX::XMFLOAT4X4 GetProjMat(bool _transpose)
	{
		mtgb::Matrix4x4 mat;
		Game::System<CameraSystem>().GetProjMatrix(&mat);
		if (_transpose)
		{
			mat = DirectX::XMMatrixTranspose(mat);
		}
		DirectX::XMFLOAT4X4 fmat;
		DirectX::XMStoreFloat4x4(&fmat, mat);
		return fmat;
	}

	DirectX::XMFLOAT4X4 GetViewMat(bool _transpose)
	{
		mtgb::Matrix4x4 mat;
		Game::System<CameraSystem>().GetViewMatrix(&mat);
		if (_transpose)
		{
			mat = DirectX::XMMatrixTranspose(mat);
		}
		DirectX::XMFLOAT4X4 fmat;
		DirectX::XMStoreFloat4x4(&fmat, mat);
		return fmat;
	}

	int32_t GetMaxFrame(const EffectRef& _effect)
	{
		return _effect->CalculateTerm().TermMax;
	}

	void EffectManager::Initialize()
	{        
		fps_ = Game::System<Screen>().GetFPS();
		
		// 描画に使用するレンダラー
		rendererRef_ = Renderer::Create(DirectX11Draw::pDevice_.Get(), DirectX11Draw::pContext_.Get(), MAX_SQUARE);

		// エフェクトを管理するマネージャー
		managerRef_ = Effekseer::Manager::Create(MAX_SQUARE);
		managerRef_->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

		// インスタンスプールを確保
		effectInstances_.reserve(kEffectPoolCapacity);

		// マネージャーに各種レンダラー、ローダーをセット
		managerRef_->SetSpriteRenderer(rendererRef_->CreateSpriteRenderer());
		managerRef_->SetRibbonRenderer(rendererRef_->CreateRibbonRenderer());
		managerRef_->SetRingRenderer(rendererRef_->CreateRingRenderer());
		managerRef_->SetTrackRenderer(rendererRef_->CreateTrackRenderer());
		managerRef_->SetModelRenderer(rendererRef_->CreateModelRenderer());

		managerRef_->SetTextureLoader(rendererRef_->CreateTextureLoader());
		managerRef_->SetModelLoader(rendererRef_->CreateModelLoader());
		managerRef_->SetMaterialLoader(rendererRef_->CreateMaterialLoader());
		managerRef_->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

		// ゲームで使用するエフェクトを登録
		RegisterEffects();
	}

	void EffectManager::Update()
	{
		double _deltaTime = Time::DeltaTime();
		for (auto& instancePtr : effectInstances_)
		{
			// 未使用のデータは更新しない
			if (!instancePtr)
			{
				continue;
			}

			auto& instance = *instancePtr;
			auto& effectParams = instance.pEffectParameters_;
			Handle& handle = instance.handle_;
			
			if (!effectParams)
			{
				instancePtr.reset();
				continue;
			}

			// 破棄フラグが立っているか
			if (effectParams->destroyMe)
			{
				// エフェクトの再生を停止
				if (handle >= 0)
				{
					managerRef_->StopEffect(handle);
				}
				// インスタンスを解放
				instancePtr.reset();
				continue;
			}

			// 再生時間が0の場合、再生を始める
			if (instance.elapsedTime_ == 0)
			{
				handle = managerRef_->Play(instance.GetEffectData()->GetEffectRef(), 0, 0, 0);
			}
			
			// エフェクトの最大フレームを取得
			int32_t maxFrame = GetMaxFrame(instance.GetEffectData()->GetEffectRef());
			// 再生終了
			double duration = maxFrame / fps_;
			if (instance.elapsedTime_ > duration)
			{
				if (handle >= 0)
				{
					// エフェクトの再生を停止
					managerRef_->StopEffect(handle);
				}

				// ループ再生するかどうか
				if (effectParams->isLoop)
				{
					// 再生時間を0にして、再び再生できるように設定
					instance.elapsedTime_ = 0;
				}
				else
				{
					// ループ再生しない場合、未使用状態に戻す
					instancePtr.reset();
				}
			}
			// エフェクトの更新を行う
			else
			{
				// フレーム数を設定
				rendererRef_->SetTime(static_cast<float>(instance.elapsedTime_));
				// エフェクトのインスタンスに対して処理をし、更新速度を設定
				managerRef_->SetMatrix(handle, CnvMat43(effectParams->worldMat));
				managerRef_->SetSpeed(handle, effectParams->speed);
				// 再生時間を増やす
				instance.elapsedTime_ += _deltaTime;
			}
		}
		
		managerRef_->Update(static_cast<float>(_deltaTime * fps_));
	}

	void EffectManager::Draw()
	{
		SetCamera();
		rendererRef_->BeginRendering();
		Effekseer::Manager::DrawParameter drawParameter;
		drawParameter.ZNear = Game::System<CameraSystem>().GetNear();
		drawParameter.ZFar = Game::System<CameraSystem>().GetFar();
		drawParameter.ViewProjectionMatrix = rendererRef_->GetCameraProjectionMatrix();
		managerRef_->Draw(drawParameter);
		rendererRef_->EndRendering();
	}

	void EffectManager::SetCamera()
	{
		rendererRef_->SetProjectionMatrix(CnvMat(GetProjMat(true)));
		rendererRef_->SetCameraMatrix(CnvMat(GetViewMat(true)));
	}

	void EffectManager::RegisterEffect(std::string_view _effectName, std::string_view _filePath)
	{
		effectList_.emplace(_effectName, std::make_shared<EffectData>(managerRef_, _filePath));
	}

	std::weak_ptr<EffectParameters> EffectManager::Play(std::string_view _effectName, const EffectParameters& _effectParameters)
	{
		if (auto iter = effectList_.find(_effectName.data()); iter != effectList_.end())
		{
			// 空きスロットを再利用
			for (auto& slot : effectInstances_)
			{
				if (!slot)
				{
					slot = std::make_unique<EffectInstance>(iter->second);
					slot->pEffectParameters_ = std::make_shared<EffectParameters>(_effectParameters);
					return std::weak_ptr<EffectParameters>(slot->pEffectParameters_);
				}
			}

			// まだ容量に余裕がある場合のみ新規に追加
			if (effectInstances_.size() < kEffectPoolCapacity)
			{
				std::unique_ptr<EffectInstance> effectInstance = std::make_unique<EffectInstance>(iter->second);
				effectInstance->pEffectParameters_ = std::make_shared<EffectParameters>(_effectParameters);
				std::weak_ptr<EffectParameters> weakRef = std::weak_ptr<EffectParameters>(effectInstance->pEffectParameters_);
				effectInstances_.emplace_back(std::move(effectInstance));
				return weakRef;
			}

			LOGIMGUI("Effect pool is full. Cannot play %s", std::string{ _effectName }.c_str());
		 }
		 // 名前に対応するエフェクトが存在しない
		 else
		 {
			 // ログに出力
			 std::string effectNameStr{ _effectName };
			 LOGIMGUI("EffectName %s is not found !",effectNameStr.c_str());
		 }

		// エフェクトの取得、作成に失敗
		return std::weak_ptr<EffectParameters>();
	}

	EffectData::EffectData(std::string_view _filePath)
		: filePath_(_filePath)
	{
	}

	EffectData::EffectData(const Effekseer::ManagerRef& _manager, std::string_view _filePath)
		: filePath_(_filePath)
	{
		Load(_manager);
	}
	

	void EffectData::Load(const Effekseer::ManagerRef& _manager)
	{
		if (fs::is_regular_file(filePath_))
		{
			fs::path filepath(filePath_);
			effectRef_ = Effekseer::Effect::Create(_manager, filepath.u16string().c_str());
		}
		EffectTerm t = effectRef_->CalculateTerm();
		t.TermMax;
		t.TermMin;
		int32_t i = t.TermMax;
	}

	std::string EffectData::GetFilePath() const
	{
		return filePath_.string();
	}

	const Effekseer::EffectRef& EffectData::GetEffectRef() const
	{
		return effectRef_;
	}

	EffectInstance::EffectInstance(const std::shared_ptr<EffectData>& _effectData)
		: pEffectData_{ _effectData}
		, elapsedTime_{0}
		, handle_{-1}
	{
	}

	std::shared_ptr<const EffectData> EffectInstance::GetEffectData() const
	{
		return pEffectData_;
	}

}