#pragma once
#include <string>
#include <functional>
#include <cstdint>
#include <map>
#include <list>
#include <cassert>
#include <typeinfo>
#include <string_view>
//#include <type_traits>
#include "ISystem.h"
#include "cmtgb.h"
#include "Vector2Int.h"


namespace mtgb
{
	using EntityId = int64_t;
	class IComponentPool;

	/// <summary>
	/// このクラスを継承し、作るゲームのクラスを作ってください
	/// </summary>
	class Game
	{
	protected:
		/// <summary>
		/// システムの更新タイミングタイプ
		/// </summary>
		enum struct SystemUpdateType : int8_t
		{
			DontCallMe,  // 呼び出さないで！
			Cycle,  // 毎サイクル
			Frame,  // 毎フレーム
			Fixed,  // 一定の間隔
		};

		/// <summary>
		/// システム登録のコールバック関数
		/// </summary>
		using RegisterSystem = std::function<void(
			const char* _systemTypeName,
			ISystem* _system,
			const bool _isComponentPool,
			const SystemUpdateType)>;

		/// <summary>
		/// システム登録の関数ホルダ
		/// </summary>
		class RegisterSystemFuncHolder
		{
		public:
			RegisterSystemFuncHolder(RegisterSystem _function) :
				function_{ _function }
			{}

			template<typename SystemT>
			void Set(SystemUpdateType _type, const bool _isComponentCP = false) const
			{
				function_(
					typeid(SystemT).name(),
					dynamic_cast<ISystem*>(new SystemT{}),
					_isComponentCP,
					_type);
			}

		private:
			RegisterSystem function_;
		};

	protected:
		Game();
		virtual ~Game();

	protected:
		/// <summary>
		/// <para>任意の更新順番でシステムを設定する</para>
		/// <para>使うシステムを必要なだけこの関数内でnewしてください</para>
		/// </summary>
		virtual void SetupSystems(const RegisterSystemFuncHolder& _registerSystem) = 0;
		/// <summary>
		/// ゲームのバージョンを必ず返してください
		/// </summary>
		/// <returns></returns>
		virtual std::string_view GetVarsion() const = 0;
		/// <summary>
		/// ゲームのタイトルを必ず返してください
		/// </summary>
		/// <returns></returns>
		virtual std::string_view GetTitle() const = 0;

		virtual Vector2Int GetScreenSize() const;

	private:
		std::map<std::string, ISystem*> pRegisterSystems_;  // 登録済みのシステム
		std::list<ISystem*> pCycleUpdateSystems_;  // 毎サイクル更新されるシステム
		std::list<ISystem*> pFrameUpdateSystems_;  // 毎フレーム更新されるシステム
		std::list<ISystem*> pFixedUpdateSystems_;  // 一定期間で更新されるシステム
		std::vector<IComponentPool*> pComponentPools_;    // コンポーネントプールのシステム

	public:
		/// <summary>
		/// ゲームを起動する
		/// </summary>
		/// <typeparam name="GameT"></typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="..._args"></param>
		template<typename GameT, typename ...Args>
		static void Run(Args... _args);

		/// <summary>
		/// ゲームを終了する
		/// </summary>
		static void Exit();

		/// <summary>
		/// システムを取得する
		/// </summary>
		/// <typeparam name="SystemT">システムの型</typeparam>
		/// <returns>システムの参照ポインタ</returns>
		template<typename SystemT>
		static inline SystemT& System();

		/// <summary>
		/// ゲームのバージョンを取得
		/// </summary>
		/// <returns>バージョン情報の文字列</returns>
		static inline std::string_view Varsion() { return pInstance_->GetVarsion(); }
		/// <summary>
		/// ゲームタイトルを取得
		/// </summary>
		/// <returns>ゲームタイトルの文字列</returns>
		static inline std::string_view Title() { return pInstance_->GetTitle(); }

		/// <summary>
		/// フレームの更新をする
		/// </summary>
		static void UpdateFrame();
		/// <summary>
		/// 定期更新をする
		/// </summary>
		static void UpdateFixed();

		/// <summary>
		/// エンティティに対応するコンポーネントを削除する
		/// </summary>
		/// <param name="_entityId"></param>
		static void RemoveEntityComponent(const EntityId _entityId);

	private:
		/// <summary>
		/// システムの初期化をする
		/// </summary>
		static void InitializeSystems(const std::list<ISystem*>& _uninitialized);

		/// <summary>
		/// システムの解放処理をする
		/// </summary>
		static void ReleaseSystems(std::list<ISystem*>& _runnings);

		/// <summary>
		/// ゲームサイクルのループを開始
		/// </summary>
		static void RunLoopGameCycle();

		static Game* pInstance_;  // 唯一のゲームインスタンス
		static bool toExit_;  // 終了フラグ
	};

	template<typename GameT, typename ...Args>
	inline void Game::Run(Args... _args)
	{
		// ゲームをインスタンスする
		pInstance_ = new GameT{ _args... };

		std::list<ISystem*> systems{};

		// 各システムの登録
		pInstance_->SetupSystems(
		{
			[&](
				const char* _systemTypeName,
				ISystem* _pSystem,
				const bool _isComponentPool,
				const SystemUpdateType _systemUpdateType)
			{
				assert(_pSystem != nullptr
					&& "ISystemへのアップキャストに失敗、継承関係がpublicになっているか確認。");

				systems.push_back(_pSystem);
				if (_isComponentPool)
				{
					pInstance_->pComponentPools_.push_back(dynamic_cast<IComponentPool*>(_pSystem));
				}
				pInstance_->pRegisterSystems_.insert({ std::string{ _systemTypeName }, _pSystem });
				switch (_systemUpdateType)
				{
				case SystemUpdateType::Cycle:
					pInstance_->pCycleUpdateSystems_.push_back(_pSystem);
					break;
				case SystemUpdateType::Frame:
					pInstance_->pFrameUpdateSystems_.push_back(_pSystem);
					break;
				case SystemUpdateType::Fixed:
					pInstance_->pFixedUpdateSystems_.push_back(_pSystem);
					break;
				case SystemUpdateType::DontCallMe:
				default:
					break;
				}
			}
		});

		// 各システムの初期化
		pInstance_->InitializeSystems(systems);

		// ゲームサイクル開始
		pInstance_->RunLoopGameCycle();

		// 解放処理
		pInstance_->ReleaseSystems(systems);
		systems.clear();

		// インスタンスの解放
		delete pInstance_;
	}

	template<typename SystemT>
	inline SystemT& Game::System()
	{
		const char* t = typeid(SystemT).name();

		ISystem* pSystem{ pInstance_->pRegisterSystems_[typeid(SystemT).name()] };

		assert(pSystem != nullptr);

		// 基底クラスがISystemであるか
		static_assert(std::is_base_of<ISystem, SystemT>().value
			&& "ISystemクラスを継承していないクラスのインスタンスは取得できません。");
		return *(dynamic_cast<SystemT*>(pSystem));
	}
}
