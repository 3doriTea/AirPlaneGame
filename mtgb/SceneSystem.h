#pragma once
#include "ISystem.h"
#include "GameScene.h"
#include "ReleaseUtility.h"

namespace mtgb
{
	class SceneSystem : public ISystem
	{
	public:
		SceneSystem();
		~SceneSystem();

		/// <summary>
		/// <para>シーン遷移をする</para>
		/// <para>NOTE: 呼んだ瞬間、遷移先クラスのコンストラクタは呼ばれるが、</para>
		/// <para>----: Initializeは次のフレームで呼ばれる。</para>
		/// </summary>
		/// <typeparam name="NextSceneT">遷移先のシーンクラスの型</typeparam>
		/// <typeparam name="...Args">遷移先クラスのコンストラクタ可変長引数</typeparam>
		/// <param name="..._args">遷移先クラスのコンストラクタ引数</param>
		template<class NextSceneT, typename ...Args>
		void Move(Args... _args);

		/// <summary>
		/// 現在のシーンのポインタの取得
		/// </summary>
		/// <returns>シーンのポインタ</returns>
		GameScene* GetActiveScene() const { return GameScene::pInstance_; }

		void Initialize() override;
		void Update() override;

	private:
		void ChangeScene();

	private:
		GameScene* pNextScene_;
	};

	template<class NextSceneT, typename ...Args>
	inline void SceneSystem::Move(Args... _args)
	{
		// 基底クラスがGameSceneであるか
		static_assert(std::is_base_of<GameScene, NextSceneT>().value
			&& "GameSceneクラスを継承していないクラスにシーン遷移できません。");
		
		// 既に次のシーンが割り当てられているなら解放する
		SAFE_DELETE(pNextScene_);

		pNextScene_ = new NextSceneT{ _args... };
	}
}
