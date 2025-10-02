#pragma once
#include <vector>
#include <queue>
#include <type_traits>
#include <functional>
#include "ShowType.h"
#include "ImGuiShowable.h"
#include "TypeRegistry.h"
#include <optional>
#include <tuple>
#include <map>
namespace mtgb
{

	struct Vector3;

	using ShowItem = std::pair<std::string, std::function<void()>>;
	using ShowQueue = std::queue<ShowItem>;

	struct ImGuiWindowState
	{
		std::string selectedName;
		bool isOpen = false;
	};
	/// <summary>
		/// ImGuiに表示をする際に使う
		/// </summary>
	class MTImGui final
	{

	public:
		static MTImGui& Instance()
		{
			static MTImGui instance;
			return instance;
		}

		void Initialize();
		void Update();

		/// <summary>
		/// ImGuiウィンドウの表示、キュー内の表示関数の一括実行、ImGuiウィンドウのEnd()までを行う
		/// </summary>
		/// <param name="_showType"></param>
		void ShowWindow(ShowType _showType);
		/// <summary>
		/// 表示キューを一括実行し、クリア
		/// </summary>
		/// <param name="show"></param>
		void ExecuteShowQueue(ShowType show);

		/// <summary>
		/// 型を指定して表示キューに積む
		/// 事前にSetupShowFuncで表示方法を登録する必要あり
		/// </summary>
		/// <typeparam name="T">表示対象の型</typeparam>
		/// <param name="target">表示対象のポインタ</param>
		/// <param name="name">表示対象の名前</param>
		/// <param name="show">表示するImGuiWindow</param>
		template<typename T>
		void TypedShow(T* target, const std::string& name, ShowType show = ShowType::Inspector);
		/// <summary>
		/// ImGuiShowable*インスタンスを登録、毎回ShowImGuiを呼ぶ
		/// ImGuiShowableは自動で登録される
		/// </summary>
		/// <param name="obj"></param>
		void Register(ImGuiShowable* obj);
		/// <summary>
		/// 登録解除
		/// デストラクタで呼ばれる
		/// </summary>
		/// <param name="obj"></param>
		void Unregister(ImGuiShowable* obj);

		/// <summary>
		/// コールバックを表示キューに直接積む
		/// </summary>
		/// <param name="func">コールバック</param>
		/// <param name="show">表示場所</param>
		void DirectShow(std::function<void()> func, const std::string& name, ShowType show);

		/// <summary>
		/// <para> ImGuiWindowに線分を描画 </para>
		/// </summary>
		/// <param name="_from">始点</param>
		/// <param name="_to">終点</param>
		/// <param name="_thickness">線の太さ</param>
		void DrawLine(const Vector3& _from, const Vector3& _to, float _thickness);

		
		/// <summary>
		/// <para> ImGuiWindowにベクトルを描画 </para>
		/// <para> 始点 + ベクトルで描画される </para>
		/// </summary>
		/// <param name="_start">始点</param>
		/// <param name="_vec">ベクトル</param>
		/// <param name="_thickness"></param>
		void DrawVec(const Vector3& _start, const Vector3 & _vec, float _thickness);

		static const char* GetName(ShowType _showType)
		{
			if (_showType == ShowType::Inspector)
			{
				return "Inspector";
			}
			if (_showType == ShowType::SceneView)
			{
				return "Game View";
			}
			if (_showType == ShowType::Settings)
			{
				return "Settings";
			}

			return "None";
		}

		void SetWindowOpen(ShowType _showType, bool _flag);
		void SetAllWindowOpen(bool _flag);
		void ChangeWindowOpen(ShowType _showType);
		void ChangeAllWindowOpen();
		void ShowLog();
	private:
		MTImGui();
		MTImGui(const MTImGui& other) = delete;
		~MTImGui();

		/// <summary>
		/// TypedShowの_targetの型に対応する関数を登録する
		/// </summary>
		void SetupShowFunc();

		void ShowListView(ShowType _show);

		std::vector<ImGuiShowable*> showableObjs_;
		
		std::map<ShowType, ShowQueue> showQueues_;
		std::map<ShowType, ImGuiWindowState> imguiWindowStates_; // ShowTypeごとのウィンドウの状態

		//std::queue<std::pair<std::string,std::function<void()>>> inspectorShowList_;
		std::queue<std::function<void()>> sceneViewShowList_;

		void DrawRayImpl(const Vector3& _start, const Vector3& _dir, float _thickness);
		void DrawLineImpl(const Vector3& _from, const Vector3& _to, float _thickness);

		bool updatingImGuiShowable_;
	};

	template<typename T>
	inline void MTImGui::TypedShow(T* target, const std::string& name, ShowType show)
	{
		using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
		//PushShowFunc( [=] {proxy->ShowImGui(std::any(target), name); }, show);
		DirectShow([=]() {TypeRegistry::Instance().CallFunc<Type>(target, name.c_str()); }, name, show);
	}
}